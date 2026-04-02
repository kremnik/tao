#pragma once

#include <cstdint>

namespace tao {

enum class MessageType : uint8_t {
    // Client -> Server
    JoinRequest  = 1,
    PaddleInput  = 2,
    LeaveMatch   = 3,

    // Server -> Client
    JoinResponse        = 10,
    WaitingForOpponent  = 11,
    MatchStarting       = 12,
    StateUpdate         = 13,
    MatchEnd            = 14,
    PlayerDisconnected  = 15,
    PlayerReconnected   = 16,
};

} // namespace tao
