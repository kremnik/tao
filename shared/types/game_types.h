#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace tao {

struct Ball {
    float x  = 0.0f;
    float y  = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    bool  active = false;
};

struct Paddle {
    float x     = 0.0f;
    float width = 100.0f;
};

struct Block {
    float x = 0.0f;
    float y = 0.0f;
    bool  alive = true;
};

enum class MatchState : uint8_t {
    WaitingForPlayers = 0,
    Playing           = 1,
    Paused            = 2,
    Finished          = 3
};

struct GameSnapshot {
    float    ballX  = 0, ballY  = 0;
    float    ballVX = 0, ballVY = 0;
    bool     ballActive = false;

    float    paddle1X = 0, paddle2X = 0;

    uint32_t score1 = 0, score2 = 0;

    MatchState state = MatchState::WaitingForPlayers;

    std::vector<bool> blocksAlive;

    uint8_t yourPlayerId = 0;
};

} // namespace tao
