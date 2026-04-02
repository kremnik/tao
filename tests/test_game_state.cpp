#include "game_state.h"
#include <cassert>
#include <iostream>

using namespace tao;

static void testInitialState() {
    GameState gs;
    auto snap = gs.snapshot();
    assert(snap.state == MatchState::Playing);
    assert(snap.ballActive == true);
    assert(snap.score1 == 0);
    assert(snap.score2 == 0);
    assert(snap.blocksAlive.size() ==
           static_cast<size_t>(constants::MAX_BLOCKS));
}

static void testPaddleClamp() {
    GameState gs;
    gs.setPaddleX(0, -100.0f);
    auto snap = gs.snapshot();
    assert(snap.paddle1X >= constants::PADDLE_WIDTH / 2.0f);

    gs.setPaddleX(0, 9999.0f);
    snap = gs.snapshot();
    assert(snap.paddle1X <= constants::FIELD_WIDTH - constants::PADDLE_WIDTH / 2.0f);
}

static void testUpdateMovesBall() {
    GameState gs;
    auto before = gs.snapshot();
    gs.update(1.0f / 60.0f);
    auto after = gs.snapshot();
    assert(before.ballX != after.ballX || before.ballY != after.ballY);
}

int main() {
    testInitialState();
    testPaddleClamp();
    testUpdateMovesBall();
    std::cout << "All game-state tests passed.\n";
    return 0;
}
