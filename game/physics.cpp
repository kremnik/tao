#include "physics.h"
#include "types/constants.h"
#include <cmath>
#include <algorithm>

namespace tao {
namespace physics {

bool reflectOffPaddle(Ball& ball, const Paddle& paddle,
                      float paddleY, float paddleHeight) {
    float r = constants::BALL_RADIUS;

    float pLeft   = paddle.x - paddle.width / 2.0f;
    float pRight  = paddle.x + paddle.width / 2.0f;
    float pTop    = paddleY;
    float pBottom = paddleY + paddleHeight;

    if (ball.x + r < pLeft  || ball.x - r > pRight)  return false;
    if (ball.y + r < pTop   || ball.y - r > pBottom)  return false;

    float speed = std::sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
    float hitPos = (ball.x - paddle.x) / (paddle.width / 2.0f);
    hitPos = std::clamp(hitPos, -1.0f, 1.0f);
    float angle = hitPos * 1.0f;

    if (ball.vy > 0 && ball.y < paddleY + paddleHeight / 2.0f) {
        ball.vx =  speed * std::sin(angle);
        ball.vy = -speed * std::cos(angle);
        ball.y  =  pTop - r;
        return true;
    }
    if (ball.vy < 0 && ball.y > paddleY + paddleHeight / 2.0f) {
        ball.vx = speed * std::sin(angle);
        ball.vy = speed * std::cos(angle);
        ball.y  = pBottom + r;
        return true;
    }
    return false;
}

bool reflectOffBlock(Ball& ball, const Block& block,
                     float blockW, float blockH) {
    float r = constants::BALL_RADIUS;

    float bLeft   = block.x;
    float bRight  = block.x + blockW;
    float bTop    = block.y;
    float bBottom = block.y + blockH;

    if (ball.x + r < bLeft  || ball.x - r > bRight)  return false;
    if (ball.y + r < bTop   || ball.y - r > bBottom)  return false;

    float overlapL = (ball.x + r) - bLeft;
    float overlapR = bRight  - (ball.x - r);
    float overlapT = (ball.y + r) - bTop;
    float overlapB = bBottom - (ball.y - r);

    float minH = std::min(overlapL, overlapR);
    float minV = std::min(overlapT, overlapB);

    if (minH < minV)
        ball.vx = -ball.vx;
    else
        ball.vy = -ball.vy;

    return true;
}

} // namespace physics
} // namespace tao
