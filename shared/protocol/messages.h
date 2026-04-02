#pragma once

#include "message_types.h"
#include "types/game_types.h"
#include <SFML/Network/Packet.hpp>
#include <string>

namespace tao {

// ---- Client -> Server ----

struct JoinRequest {
    std::string playerName;
    std::string sessionToken;
};

struct PaddleInput {
    float paddleX = 0.0f;
};

// ---- Server -> Client ----

struct JoinResponse {
    bool        accepted = false;
    uint8_t     playerId = 0;
    std::string sessionToken;
    std::string rejectReason;
};

struct MatchStarting {
    GameSnapshot initialState;
};

struct MatchEnd {
    uint8_t  winnerId = 0;
    uint32_t score1   = 0;
    uint32_t score2   = 0;
};

struct PlayerConnectionEvent {
    uint8_t playerId = 0;
};

// ---- Serialization operators ----

sf::Packet& operator<<(sf::Packet& p, const JoinRequest& m);
sf::Packet& operator>>(sf::Packet& p, JoinRequest& m);

sf::Packet& operator<<(sf::Packet& p, const PaddleInput& m);
sf::Packet& operator>>(sf::Packet& p, PaddleInput& m);

sf::Packet& operator<<(sf::Packet& p, const JoinResponse& m);
sf::Packet& operator>>(sf::Packet& p, JoinResponse& m);

sf::Packet& operator<<(sf::Packet& p, const GameSnapshot& s);
sf::Packet& operator>>(sf::Packet& p, GameSnapshot& s);

sf::Packet& operator<<(sf::Packet& p, const MatchStarting& m);
sf::Packet& operator>>(sf::Packet& p, MatchStarting& m);

sf::Packet& operator<<(sf::Packet& p, const MatchEnd& m);
sf::Packet& operator>>(sf::Packet& p, MatchEnd& m);

sf::Packet& operator<<(sf::Packet& p, const PlayerConnectionEvent& m);
sf::Packet& operator>>(sf::Packet& p, PlayerConnectionEvent& m);

sf::Packet makePacket(MessageType type);

} // namespace tao
