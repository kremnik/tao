#include "protocol/messages.h"
#include <cassert>
#include <iostream>

using namespace tao;

static void testJoinRequestRoundTrip() {
    JoinRequest req;
    req.playerName   = "Alice";
    req.sessionToken = "abc123";

    sf::Packet pkt;
    pkt << req;

    JoinRequest out;
    pkt >> out;
    assert(out.playerName   == "Alice");
    assert(out.sessionToken == "abc123");
}

static void testGameSnapshotRoundTrip() {
    GameSnapshot snap;
    snap.ballX = 100.5f;  snap.ballY = 200.0f;
    snap.ballVX = -1.0f;  snap.ballVY = 2.0f;
    snap.ballActive = true;
    snap.paddle1X = 300.0f; snap.paddle2X = 400.0f;
    snap.score1 = 42; snap.score2 = 7;
    snap.state = MatchState::Playing;
    snap.yourPlayerId = 1;
    snap.blocksAlive = {true, false, true, true, false};

    sf::Packet pkt;
    pkt << snap;

    GameSnapshot out;
    pkt >> out;
    assert(out.ballX == snap.ballX);
    assert(out.ballActive == true);
    assert(out.score1 == 42);
    assert(out.state == MatchState::Playing);
    assert(out.blocksAlive.size() == 5);
    assert(out.blocksAlive[1] == false);
    assert(out.blocksAlive[2] == true);
}

static void testMatchEndRoundTrip() {
    MatchEnd me;
    me.winnerId = 2;
    me.score1 = 100; me.score2 = 200;

    sf::Packet pkt;
    pkt << me;

    MatchEnd out;
    pkt >> out;
    assert(out.winnerId == 2);
    assert(out.score1 == 100);
    assert(out.score2 == 200);
}

int main() {
    testJoinRequestRoundTrip();
    testGameSnapshotRoundTrip();
    testMatchEndRoundTrip();
    std::cout << "All protocol tests passed.\n";
    return 0;
}
