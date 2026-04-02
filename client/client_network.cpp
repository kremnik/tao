#include "client_network.h"

namespace tao {

bool ClientNetwork::connect(const std::string& host, uint16_t port) {
    socket_.setBlocking(true);
    if (socket_.connect(host, port, sf::seconds(3)) != sf::Socket::Done)
        return false;
    socket_.setBlocking(false);
    connected_ = true;
    return true;
}

void ClientNetwork::disconnect() {
    socket_.disconnect();
    connected_ = false;
}

void ClientNetwork::sendJoinRequest(const std::string& name,
                                    const std::string& token) {
    if (!connected_) return;
    auto pkt = makePacket(MessageType::JoinRequest);
    JoinRequest req;
    req.playerName   = name;
    req.sessionToken = token;
    pkt << req;
    socket_.setBlocking(true);
    socket_.send(pkt);
    socket_.setBlocking(false);
}

void ClientNetwork::sendPaddleInput(float x) {
    if (!connected_) return;
    auto pkt = makePacket(MessageType::PaddleInput);
    PaddleInput inp;
    inp.paddleX = x;
    pkt << inp;
    socket_.send(pkt);
}

void ClientNetwork::sendLeave() {
    if (!connected_) return;
    auto pkt = makePacket(MessageType::LeaveMatch);
    socket_.setBlocking(true);
    socket_.send(pkt);
    socket_.setBlocking(false);
}

std::vector<ServerMessage> ClientNetwork::poll() {
    std::vector<ServerMessage> msgs;
    if (!connected_) return msgs;

    while (true) {
        sf::Packet pkt;
        auto st = socket_.receive(pkt);
        if (st == sf::Socket::Done) {
            uint8_t t;
            pkt >> t;
            ServerMessage sm{};
            sm.type = static_cast<MessageType>(t);

            switch (sm.type) {
            case MessageType::JoinResponse:
                pkt >> sm.joinResponse;  break;
            case MessageType::WaitingForOpponent:
                break;
            case MessageType::MatchStarting:
                pkt >> sm.matchStarting; break;
            case MessageType::StateUpdate:
                pkt >> sm.stateUpdate;   break;
            case MessageType::MatchEnd:
                pkt >> sm.matchEnd;      break;
            case MessageType::PlayerDisconnected:
            case MessageType::PlayerReconnected:
                pkt >> sm.connectionEvent; break;
            default: break;
            }
            msgs.push_back(std::move(sm));
        } else if (st == sf::Socket::Disconnected) {
            connected_ = false;
            break;
        } else {
            break;
        }
    }
    return msgs;
}

} // namespace tao
