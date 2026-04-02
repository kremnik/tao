#pragma once

#include "game_state.h"
#include "types/game_types.h"

namespace tao {

class Match {
public:
    void start();
    void update(float dt);
    void handleInput(int playerId, float paddleX);

    void playerDisconnected(int playerId);
    void playerReconnected(int playerId);

    GameSnapshot snapshot() const;
    bool         isFinished() const;
    bool         isForfeited() const;
    uint8_t      winnerId() const;
    uint32_t     score(int playerId) const;
    MatchState   state() const;

private:
    GameState gameState_;
    bool      playerConnected_[2] = {true, true};
    float     disconnectTimer_    = 0.0f;
    int       disconnectedPlayer_ = -1;
    bool      forfeited_          = false;
    uint8_t   forfeitWinner_      = 0;
};

} // namespace tao
