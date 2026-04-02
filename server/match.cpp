#include "match.h"
#include "types/constants.h"

namespace tao {

void Match::start() {
    gameState_.init();
    playerConnected_[0] = playerConnected_[1] = true;
    disconnectedPlayer_ = -1;
    disconnectTimer_ = 0.0f;
    forfeited_ = false;
}

void Match::update(float dt) {
    if (forfeited_) return;

    if (disconnectedPlayer_ >= 0) {
        disconnectTimer_ += dt;
        if (disconnectTimer_ >= constants::RECONNECT_TIMEOUT) {
            forfeited_ = true;
            forfeitWinner_ = static_cast<uint8_t>(2 - disconnectedPlayer_);
            gameState_.setMatchState(MatchState::Finished);
        }
        return;
    }

    gameState_.update(dt);
}

void Match::handleInput(int playerId, float paddleX) {
    gameState_.setPaddleX(playerId, paddleX);
}

void Match::playerDisconnected(int playerId) {
    if (playerId < 0 || playerId > 1) return;
    playerConnected_[playerId] = false;
    disconnectedPlayer_ = playerId;
    disconnectTimer_ = 0.0f;
    gameState_.setMatchState(MatchState::Paused);
}

void Match::playerReconnected(int playerId) {
    if (playerId < 0 || playerId > 1) return;
    playerConnected_[playerId] = true;
    disconnectedPlayer_ = -1;
    disconnectTimer_ = 0.0f;
    gameState_.setMatchState(MatchState::Playing);
}

GameSnapshot Match::snapshot() const { return gameState_.snapshot(); }
bool Match::isFinished()     const { return forfeited_ || gameState_.isFinished(); }
bool Match::isForfeited()    const { return forfeited_; }

uint8_t Match::winnerId() const {
    if (forfeited_) return forfeitWinner_;
    return gameState_.winnerId();
}

uint32_t Match::score(int playerId) const { return gameState_.score(playerId); }
MatchState Match::state()    const { return gameState_.matchState(); }

} // namespace tao
