#include "protocol/messages.h"

namespace tao {

// ---- JoinRequest ----

sf::Packet& operator<<(sf::Packet& p, const JoinRequest& m) {
    return p << m.playerName << m.sessionToken;
}

sf::Packet& operator>>(sf::Packet& p, JoinRequest& m) {
    return p >> m.playerName >> m.sessionToken;
}

// ---- PaddleInput ----

sf::Packet& operator<<(sf::Packet& p, const PaddleInput& m) {
    return p << m.paddleX;
}

sf::Packet& operator>>(sf::Packet& p, PaddleInput& m) {
    return p >> m.paddleX;
}

// ---- JoinResponse ----

sf::Packet& operator<<(sf::Packet& p, const JoinResponse& m) {
    p << m.accepted << m.playerId << m.sessionToken;
    if (!m.accepted)
        p << m.rejectReason;
    return p;
}

sf::Packet& operator>>(sf::Packet& p, JoinResponse& m) {
    p >> m.accepted >> m.playerId >> m.sessionToken;
    if (!m.accepted)
        p >> m.rejectReason;
    return p;
}

// ---- GameSnapshot ----

sf::Packet& operator<<(sf::Packet& p, const GameSnapshot& s) {
    p << s.ballX << s.ballY << s.ballVX << s.ballVY << s.ballActive;
    p << s.paddle1X << s.paddle2X;
    p << s.score1 << s.score2;
    p << static_cast<uint8_t>(s.state);
    p << s.yourPlayerId;

    auto blockCount = static_cast<uint16_t>(s.blocksAlive.size());
    p << blockCount;
    for (uint16_t i = 0; i < blockCount; ++i)
        p << static_cast<uint8_t>(s.blocksAlive[i] ? 1 : 0);
    return p;
}

sf::Packet& operator>>(sf::Packet& p, GameSnapshot& s) {
    p >> s.ballX >> s.ballY >> s.ballVX >> s.ballVY >> s.ballActive;
    p >> s.paddle1X >> s.paddle2X;
    p >> s.score1 >> s.score2;

    uint8_t stateRaw;
    p >> stateRaw;
    s.state = static_cast<MatchState>(stateRaw);

    p >> s.yourPlayerId;

    uint16_t blockCount;
    p >> blockCount;
    s.blocksAlive.resize(blockCount);
    for (uint16_t i = 0; i < blockCount; ++i) {
        uint8_t alive;
        p >> alive;
        s.blocksAlive[i] = (alive != 0);
    }
    return p;
}

// ---- MatchStarting ----

sf::Packet& operator<<(sf::Packet& p, const MatchStarting& m) {
    return p << m.initialState;
}

sf::Packet& operator>>(sf::Packet& p, MatchStarting& m) {
    return p >> m.initialState;
}

// ---- MatchEnd ----

sf::Packet& operator<<(sf::Packet& p, const MatchEnd& m) {
    return p << m.winnerId << m.score1 << m.score2;
}

sf::Packet& operator>>(sf::Packet& p, MatchEnd& m) {
    return p >> m.winnerId >> m.score1 >> m.score2;
}

// ---- PlayerConnectionEvent ----

sf::Packet& operator<<(sf::Packet& p, const PlayerConnectionEvent& m) {
    return p << m.playerId;
}

sf::Packet& operator>>(sf::Packet& p, PlayerConnectionEvent& m) {
    return p >> m.playerId;
}

// ---- Helper ----

sf::Packet makePacket(MessageType type) {
    sf::Packet pkt;
    pkt << static_cast<uint8_t>(type);
    return pkt;
}

} // namespace tao
