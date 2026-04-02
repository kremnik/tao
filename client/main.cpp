#include "client_network.h"
#include "renderer.h"
#include "types/constants.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    uint16_t    port = tao::constants::DEFAULT_PORT;
    std::string name = "Player";
    std::string initialToken;

    if (argc > 1) host = argv[1];
    if (argc > 2) port = static_cast<uint16_t>(std::atoi(argv[2]));
    if (argc > 3) name = argv[3];
    if (argc > 4) initialToken = argv[4];

    std::cout.setf(std::ios::unitbuf);
    std::cout << "Tao Arkanoid Client (" << name << ")" << std::endl;

    tao::Renderer renderer;
    if (!renderer.init("Arkanoid - " + name)) return 1;

    tao::ClientNetwork net;
    tao::GameSnapshot  state;
    std::string sessionToken;
    uint8_t     myId    = 0;
    bool        inMatch = false;
    std::string status  = "Connecting...";

    if (!initialToken.empty())
        sessionToken = initialToken;

    if (net.connect(host, port)) {
        net.sendJoinRequest(name, sessionToken);
        status = "Connected, joining...";
    } else {
        status = "Cannot connect to " + host + ":" + std::to_string(port);
    }

    sf::Clock reconnectClock;

    while (renderer.isOpen()) {
        // Events
        sf::Event ev;
        while (renderer.window().pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                renderer.window().close();
        }

        // Input
        if (inMatch && net.isConnected()) {
            auto mouseX = static_cast<float>(
                sf::Mouse::getPosition(renderer.window()).x);
            net.sendPaddleInput(mouseX);
        }

        // Network
        if (net.isConnected()) {
            for (auto& msg : net.poll()) {
                switch (msg.type) {
                case tao::MessageType::JoinResponse:
                    if (msg.joinResponse.accepted) {
                        myId = msg.joinResponse.playerId;
                        sessionToken = msg.joinResponse.sessionToken;
                        status = "Joined as Player "
                               + std::to_string(myId);
                    } else {
                        status = "Rejected: "
                               + msg.joinResponse.rejectReason;
                    }
                    break;
                case tao::MessageType::WaitingForOpponent:
                    status = "Waiting for opponent...";
                    break;
                case tao::MessageType::MatchStarting:
                    state   = msg.matchStarting.initialState;
                    myId    = state.yourPlayerId;
                    inMatch = true;
                    status  = "Match started!";
                    break;
                case tao::MessageType::StateUpdate:
                    state   = msg.stateUpdate;
                    inMatch = true;
                    break;
                case tao::MessageType::MatchEnd: {
                    inMatch = false;
                    auto& me = msg.matchEnd;
                    if (me.winnerId == myId)
                        status = "You win!  "
                               + std::to_string(me.score1) + " - "
                               + std::to_string(me.score2);
                    else if (me.winnerId == 0)
                        status = "Draw!  "
                               + std::to_string(me.score1) + " - "
                               + std::to_string(me.score2);
                    else
                        status = "You lose!  "
                               + std::to_string(me.score1) + " - "
                               + std::to_string(me.score2);
                    break;
                }
                case tao::MessageType::PlayerDisconnected:
                    status = "Opponent disconnected — waiting for reconnect...";
                    break;
                case tao::MessageType::PlayerReconnected:
                    status = "Opponent reconnected!";
                    break;
                default: break;
                }
            }
        } else {
            if (reconnectClock.getElapsedTime().asSeconds() > 2.0f) {
                reconnectClock.restart();
                status = "Reconnecting...";
                if (net.connect(host, port)) {
                    net.sendJoinRequest(name, sessionToken);
                    status = "Reconnected, re-joining...";
                }
            }
        }

        renderer.render(state, net.isConnected(), status, myId);
    }

    if (net.isConnected()) {
        net.sendLeave();
        net.disconnect();
    }
    return 0;
}
