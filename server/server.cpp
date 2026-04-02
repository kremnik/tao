#include "server.h"
#include "match.h"
#include "leaderboard.h"
#include "protocol/messages.h"
#include "types/constants.h"
#include <iostream>
#include <random>
#include <algorithm>

namespace tao {

Server::Server()
    : leaderboard_(std::make_unique<Leaderboard>()) {
    leaderboard_->load("leaderboard.txt");
    leaderboard_->print();
}

Server::~Server() { stop(); }

bool Server::start(uint16_t port) {
    if (listener_.listen(port) != sf::Socket::Done) {
        std::cerr << "Cannot listen on port " << port << std::endl;
        return false;
    }
    running_ = true;
    acceptThread_ = std::thread(&Server::acceptLoop, this);
    std::cout << "Server listening on port " << port << std::endl;
    return true;
}

void Server::run() {
    clock_.restart();
    while (running_) {
        float dt = clock_.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        collectNewConnections();
        handlePending();
        handlePlayers();

        if (match_) {
            updateMatch(dt);
            stateAccumulator_ += dt;
            if (stateAccumulator_ >= constants::STATE_SEND_INTERVAL) {
                stateAccumulator_ -= constants::STATE_SEND_INTERVAL;
                broadcastState();
            }
        }

        sf::sleep(sf::milliseconds(
            static_cast<sf::Int32>(1000.0f / constants::SERVER_TICK_RATE)));
    }
}

void Server::stop() {
    running_ = false;
    listener_.close();
    if (acceptThread_.joinable())
        acceptThread_.join();
}

// ---------- accept thread ----------

void Server::acceptLoop() {
    while (running_) {
        auto sock = std::make_unique<sf::TcpSocket>();
        if (listener_.accept(*sock) == sf::Socket::Done) {
            std::cout << "Connection from "
                      << sock->getRemoteAddress() << std::endl;
            sock->setBlocking(false);
            std::lock_guard<std::mutex> lock(newConnMutex_);
            newConnections_.push_back(std::move(sock));
        }
    }
}

void Server::collectNewConnections() {
    std::lock_guard<std::mutex> lock(newConnMutex_);
    for (auto& sock : newConnections_)
        pending_.push_back({std::move(sock)});
    newConnections_.clear();
}

// ---------- networking ----------

void Server::handlePending() {
    for (auto it = pending_.begin(); it != pending_.end(); ) {
        sf::Packet pkt;
        auto st = it->socket->receive(pkt);
        if (st == sf::Socket::Done) {
            uint8_t t; pkt >> t;
            if (static_cast<MessageType>(t) == MessageType::JoinRequest) {
                JoinRequest req; pkt >> req;
                handleJoin(std::move(it->socket), req);
            }
            it = pending_.erase(it);
        } else if (st == sf::Socket::Disconnected) {
            it = pending_.erase(it);
        } else {
            ++it;
        }
    }
}

void Server::handlePlayers() {
    for (int i = 0; i < 2; ++i) {
        if (!players_[i].assigned || !players_[i].connected) continue;

        sf::Packet pkt;
        auto st = players_[i].socket->receive(pkt);
        if (st == sf::Socket::Done) {
            uint8_t t; pkt >> t;
            switch (static_cast<MessageType>(t)) {
            case MessageType::PaddleInput: {
                PaddleInput inp; pkt >> inp;
                if (match_) match_->handleInput(i, inp.paddleX);
                break;
            }
            case MessageType::LeaveMatch:
                std::cout << "Player " << (i+1) << " left" << std::endl;
                removePlayer(i);
                break;
            default: break;
            }
        } else if (st == sf::Socket::Disconnected) {
            std::cout << "Player " << (i+1) << " (" << players_[i].name
                      << ") disconnected" << std::endl;
            players_[i].connected = false;

            if (match_) {
                match_->playerDisconnected(i);
                int other = 1 - i;
                if (players_[other].assigned && players_[other].connected) {
                    auto p = makePacket(MessageType::PlayerDisconnected);
                    PlayerConnectionEvent ev;
                    ev.playerId = static_cast<uint8_t>(i + 1);
                    p << ev;
                    players_[other].socket->send(p);
                }
            }
        }
    }
}

// ---------- join / reconnect ----------

void Server::handleJoin(std::unique_ptr<sf::TcpSocket> sock,
                        const JoinRequest& req) {
    if (!req.sessionToken.empty()) {
        int slot = findSlotByToken(req.sessionToken);
        if (slot >= 0 && players_[slot].assigned && !players_[slot].connected) {
            std::cout << req.playerName << " reconnecting to slot "
                      << (slot+1) << std::endl;

            players_[slot].socket = std::move(sock);
            players_[slot].connected = true;

            JoinResponse resp;
            resp.accepted = true;
            resp.playerId = static_cast<uint8_t>(slot + 1);
            resp.sessionToken = req.sessionToken;
            auto p = makePacket(MessageType::JoinResponse);
            p << resp;
            players_[slot].socket->send(p);

            if (match_) {
                match_->playerReconnected(slot);
                auto sp = makePacket(MessageType::StateUpdate);
                auto snap = match_->snapshot();
                snap.yourPlayerId = static_cast<uint8_t>(slot + 1);
                sp << snap;
                players_[slot].socket->send(sp);

                int other = 1 - slot;
                if (players_[other].assigned && players_[other].connected) {
                    auto np = makePacket(MessageType::PlayerReconnected);
                    PlayerConnectionEvent ev;
                    ev.playerId = static_cast<uint8_t>(slot + 1);
                    np << ev;
                    players_[other].socket->send(np);
                }
            }
            return;
        }
    }

    int slot = -1;
    for (int i = 0; i < 2; ++i) {
        if (!players_[i].assigned) { slot = i; break; }
    }
    if (slot < 0) {
        JoinResponse resp;
        resp.accepted = false;
        resp.rejectReason = "Server full";
        auto p = makePacket(MessageType::JoinResponse);
        p << resp;
        sock->send(p);
        return;
    }

    players_[slot].socket       = std::move(sock);
    players_[slot].name         = req.playerName;
    players_[slot].sessionToken = generateToken();
    players_[slot].connected    = true;
    players_[slot].assigned     = true;

    JoinResponse resp;
    resp.accepted     = true;
    resp.playerId     = static_cast<uint8_t>(slot + 1);
    resp.sessionToken = players_[slot].sessionToken;
    auto p = makePacket(MessageType::JoinResponse);
    p << resp;
    players_[slot].socket->send(p);

    std::cout << req.playerName << " joined as Player " << (slot+1)
              << " token=" << players_[slot].sessionToken << std::endl;

    auto wp = makePacket(MessageType::WaitingForOpponent);
    players_[slot].socket->send(wp);

    tryStartMatch();
}

void Server::tryStartMatch() {
    if (match_) return;
    if (!players_[0].assigned || !players_[0].connected) return;
    if (!players_[1].assigned || !players_[1].connected) return;

    std::cout << "Both players ready - starting match" << std::endl;
    match_ = std::make_unique<Match>();
    match_->start();

    auto snap = match_->snapshot();
    for (int i = 0; i < 2; ++i) {
        auto pkt = makePacket(MessageType::MatchStarting);
        MatchStarting ms;
        ms.initialState = snap;
        ms.initialState.yourPlayerId = static_cast<uint8_t>(i + 1);
        pkt << ms;
        players_[i].socket->send(pkt);
    }
}

// ---------- game loop ----------

void Server::updateMatch(float dt) {
    if (!match_ || match_->isFinished()) {
        if (match_ && match_->isFinished()) finishMatch();
        return;
    }
    match_->update(dt);
    if (match_->isFinished()) finishMatch();
}

void Server::broadcastState() {
    if (!match_) return;
    auto snap = match_->snapshot();

    for (int i = 0; i < 2; ++i) {
        if (!players_[i].assigned || !players_[i].connected) continue;
        auto pkt = makePacket(MessageType::StateUpdate);
        GameSnapshot ps = snap;
        ps.yourPlayerId = static_cast<uint8_t>(i + 1);
        pkt << ps;
        players_[i].socket->send(pkt);
    }
}

void Server::finishMatch() {
    MatchEnd me;
    me.winnerId = match_->winnerId();
    me.score1   = match_->score(0);
    me.score2   = match_->score(1);

    for (int i = 0; i < 2; ++i) {
        if (!players_[i].assigned || !players_[i].connected) continue;
        auto pkt = makePacket(MessageType::MatchEnd);
        pkt << me;
        players_[i].socket->send(pkt);
    }

    leaderboard_->addResult(players_[0].name, players_[1].name,
                            me.score1, me.score2);
    leaderboard_->save("leaderboard.txt");
    leaderboard_->print();

    std::cout << "Match finished. Winner: Player "
              << static_cast<int>(me.winnerId) << std::endl;

    match_.reset();
    for (auto& ps : players_)
        ps = PlayerSlot{};
}

void Server::removePlayer(int slot) {
    if (slot < 0 || slot > 1) return;
    players_[slot] = PlayerSlot{};
}

// ---------- helpers ----------

std::string Server::generateToken() {
    static std::mt19937 rng(std::random_device{}());
    static const char hex[] = "0123456789abcdef";
    std::string tok(constants::SESSION_TOKEN_LENGTH, '0');
    for (auto& c : tok) c = hex[rng() % 16];
    return tok;
}

int Server::findSlotByToken(const std::string& token) {
    for (int i = 0; i < 2; ++i)
        if (players_[i].assigned && players_[i].sessionToken == token)
            return i;
    return -1;
}

} // namespace tao
