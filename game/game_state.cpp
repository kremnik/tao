#include "game_state.h"
#include "physics.h"
#include <cmath>

namespace tao {

GameState::GameState() { init(); }

void GameState::init() {
    scores_[0] = scores_[1] = 0;

    paddles_[0].x     = constants::FIELD_WIDTH / 2.0f;
    paddles_[0].width = constants::PADDLE_WIDTH;
    paddles_[1].x     = constants::FIELD_WIDTH / 2.0f;
    paddles_[1].width = constants::PADDLE_WIDTH;

    initBlocks();
    resetBall();
    state_ = MatchState::Playing;
}

void GameState::initBlocks() {
    blocks_.clear();
    float totalW = constants::BLOCK_COLUMNS * constants::BLOCK_WIDTH
                 + (constants::BLOCK_COLUMNS - 1) * constants::BLOCK_GAP;
    float startX = (constants::FIELD_WIDTH - totalW) / 2.0f;

    for (int row = 0; row < constants::BLOCK_ROWS; ++row) {
        for (int col = 0; col < constants::BLOCK_COLUMNS; ++col) {
            Block b;
            b.x     = startX + col * (constants::BLOCK_WIDTH + constants::BLOCK_GAP);
            b.y     = constants::BLOCK_AREA_TOP
                    + row * (constants::BLOCK_HEIGHT + constants::BLOCK_GAP);
            b.alive = true;
            blocks_.push_back(b);
        }
    }
}

void GameState::resetBall() {
    ball_.x  = constants::FIELD_WIDTH  / 2.0f;
    ball_.y  = constants::FIELD_HEIGHT / 2.0f;
    ball_.vx = constants::BALL_INITIAL_SPEED * 0.7f;
    ball_.vy = constants::BALL_INITIAL_SPEED * 0.7f;
    ball_.active = true;
    lastHitBy_ = -1;
}

void GameState::update(float dt) {
    if (state_ != MatchState::Playing || !ball_.active) return;

    ball_.x += ball_.vx * dt;
    ball_.y += ball_.vy * dt;

    // Left / right walls
    if (ball_.x - constants::BALL_RADIUS < 0.0f) {
        ball_.x  = constants::BALL_RADIUS;
        ball_.vx = std::abs(ball_.vx);
    }
    if (ball_.x + constants::BALL_RADIUS > constants::FIELD_WIDTH) {
        ball_.x  = constants::FIELD_WIDTH - constants::BALL_RADIUS;
        ball_.vx = -std::abs(ball_.vx);
    }

    // Paddle collisions
    if (physics::reflectOffPaddle(ball_, paddles_[0],
            constants::PADDLE1_Y, constants::PADDLE_HEIGHT))
        lastHitBy_ = 0;

    if (physics::reflectOffPaddle(ball_, paddles_[1],
            constants::PADDLE2_Y, constants::PADDLE_HEIGHT))
        lastHitBy_ = 1;

    // Block collisions
    for (auto& block : blocks_) {
        if (!block.alive) continue;
        if (physics::reflectOffBlock(ball_, block,
                constants::BLOCK_WIDTH, constants::BLOCK_HEIGHT)) {
            block.alive = false;
            if (lastHitBy_ >= 0)
                scores_[lastHitBy_] += constants::BLOCK_SCORE_VALUE;
        }
    }

    // Ball out top -> player 1 scores
    if (ball_.y + constants::BALL_RADIUS < 0.0f) {
        scores_[0] += constants::MISS_SCORE_VALUE;
        resetBall();
    }
    // Ball out bottom -> player 2 scores
    if (ball_.y - constants::BALL_RADIUS > constants::FIELD_HEIGHT) {
        scores_[1] += constants::MISS_SCORE_VALUE;
        resetBall();
    }

    // All blocks destroyed -> match finished
    bool allGone = true;
    for (const auto& b : blocks_)
        if (b.alive) { allGone = false; break; }
    if (allGone)
        state_ = MatchState::Finished;
}

void GameState::setPaddleX(int playerId, float x) {
    if (playerId < 0 || playerId > 1) return;
    float half = paddles_[playerId].width / 2.0f;
    x = std::max(half, std::min(constants::FIELD_WIDTH - half, x));
    paddles_[playerId].x = x;
}

GameSnapshot GameState::snapshot() const {
    GameSnapshot s;
    s.ballX = ball_.x;  s.ballY = ball_.y;
    s.ballVX = ball_.vx; s.ballVY = ball_.vy;
    s.ballActive = ball_.active;
    s.paddle1X = paddles_[0].x;
    s.paddle2X = paddles_[1].x;
    s.score1 = scores_[0];
    s.score2 = scores_[1];
    s.state  = state_;

    s.blocksAlive.resize(blocks_.size());
    for (size_t i = 0; i < blocks_.size(); ++i)
        s.blocksAlive[i] = blocks_[i].alive;
    return s;
}

void GameState::loadSnapshot(const GameSnapshot& snap) {
    ball_.x  = snap.ballX;  ball_.y  = snap.ballY;
    ball_.vx = snap.ballVX; ball_.vy = snap.ballVY;
    ball_.active = snap.ballActive;
    paddles_[0].x = snap.paddle1X;
    paddles_[1].x = snap.paddle2X;
    scores_[0] = snap.score1;
    scores_[1] = snap.score2;
    state_ = snap.state;

    for (size_t i = 0; i < snap.blocksAlive.size() && i < blocks_.size(); ++i)
        blocks_[i].alive = snap.blocksAlive[i];
}

bool GameState::isFinished() const {
    return state_ == MatchState::Finished;
}

uint8_t GameState::winnerId() const {
    if (scores_[0] > scores_[1]) return 1;
    if (scores_[1] > scores_[0]) return 2;
    return 0;
}

uint32_t GameState::score(int playerId) const {
    if (playerId < 0 || playerId > 1) return 0;
    return scores_[playerId];
}

} // namespace tao
