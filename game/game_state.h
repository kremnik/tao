#pragma once

#include "types/game_types.h"
#include "types/constants.h"
#include <vector>

namespace tao {

class GameState {
public:
    GameState();

    void init();
    void update(float dt);
    void setPaddleX(int playerId, float x);

    GameSnapshot snapshot() const;
    void loadSnapshot(const GameSnapshot& snap);

    bool     isFinished() const;
    uint8_t  winnerId()   const;
    uint32_t score(int playerId) const;

    MatchState matchState() const { return state_; }
    void       setMatchState(MatchState s) { state_ = s; }

private:
    void resetBall();
    void initBlocks();

    Ball               ball_;
    Paddle             paddles_[2];
    std::vector<Block> blocks_;
    uint32_t           scores_[2] = {0, 0};
    MatchState         state_ = MatchState::WaitingForPlayers;
    int                lastHitBy_ = -1;
};

} // namespace tao
