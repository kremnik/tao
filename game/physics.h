#pragma once

#include "types/game_types.h"

namespace tao {
namespace physics {

bool reflectOffPaddle(Ball& ball, const Paddle& paddle,
                      float paddleY, float paddleHeight);

bool reflectOffBlock(Ball& ball, const Block& block,
                     float blockW, float blockH);

} // namespace physics
} // namespace tao
