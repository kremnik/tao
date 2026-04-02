#pragma once

#include <cstdint>

namespace tao {
namespace constants {

constexpr float FIELD_WIDTH  = 800.0f;
constexpr float FIELD_HEIGHT = 600.0f;

constexpr float PADDLE_WIDTH  = 100.0f;
constexpr float PADDLE_HEIGHT = 15.0f;
constexpr float PADDLE1_Y     = 560.0f;
constexpr float PADDLE2_Y     = 25.0f;

constexpr float BALL_RADIUS        = 8.0f;
constexpr float BALL_INITIAL_SPEED = 300.0f;

constexpr int   BLOCK_COLUMNS  = 10;
constexpr int   BLOCK_ROWS     = 4;
constexpr int   MAX_BLOCKS     = BLOCK_COLUMNS * BLOCK_ROWS;
constexpr float BLOCK_WIDTH    = 70.0f;
constexpr float BLOCK_HEIGHT   = 20.0f;
constexpr float BLOCK_GAP      = 4.0f;
constexpr float BLOCK_AREA_TOP = 230.0f;

constexpr int BLOCK_SCORE_VALUE = 10;
constexpr int MISS_SCORE_VALUE  = 50;

constexpr uint16_t DEFAULT_PORT       = 54321;
constexpr float    SERVER_TICK_RATE   = 60.0f;
constexpr float    STATE_SEND_INTERVAL = 1.0f / 30.0f;

constexpr float RECONNECT_TIMEOUT    = 30.0f;
constexpr int   SESSION_TOKEN_LENGTH = 16;

} // namespace constants
} // namespace tao
