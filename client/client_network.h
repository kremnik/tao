#pragma once

#include "protocol/messages.h"
#include <SFML/Network.hpp>
#include <string>
#include <vector>

namespace tao {

struct ServerMessage {
    MessageType           type;
    JoinResponse          joinResponse;
    GameSnapshot          stateUpdate;
    MatchStarting         matchStarting;
    MatchEnd              matchEnd;
    PlayerConnectionEvent connectionEvent;
};

class ClientNetwork {
public:
    bool connect(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const { return connected_; }

    void sendJoinRequest(const std::string& name,
                         const std::string& token = "");
    void sendPaddleInput(float x);
    void sendLeave();

    std::vector<ServerMessage> poll();

private:
    sf::TcpSocket socket_;
    bool          connected_ = false;
};

} // namespace tao
