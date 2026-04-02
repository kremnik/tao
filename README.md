# Tao Arkanoid

Networked 2-player Arkanoid built in C++17 with an authoritative server,
two graphical clients, and a shared protocol layer.

## Gameplay

Two players face each other across a field of destructible blocks.
Player 1 controls the bottom paddle, Player 2 controls the top paddle.
A ball bounces between the paddles; hitting a block destroys it and awards
points to the player who last touched the ball. If the ball escapes past
a paddle, the opponent scores. The match ends when all blocks are destroyed.

- Field: 800 x 600
- 10 x 4 grid of blocks in the center
- Block hit: +10 points
- Ball miss: +50 points for the opponent
- Paddle control: mouse position

## Architecture

```
┌──────────────────────┐         ┌──────────────────────┐
│       Server         │◄──TCP──►│       Client         │
│                      │         │                      │
│  Accept thread       │         │  sf::RenderWindow    │
│  Game loop (60 Hz)   │         │  Mouse input         │
│  State broadcast     │         │  Non-blocking recv   │
│  (30 Hz)             │         │                      │
│  ┌────────────────┐  │         │  Renders:            │
│  │ Match           │  │  state  │   ball, paddles,     │
│  │  └─ GameState  │──┼────────►│   blocks, scores,    │
│  │  └─ Physics    │  │         │   connection status  │
│  └────────────────┘  │         └──────────────────────┘
│  Leaderboard (file)  │◄─input──  PaddleInput
└──────────────────────┘
```

The server is the **single source of truth**. Clients send paddle input and
render whatever state the server broadcasts. No client-side prediction.

### Module layout

| Directory | Library / Executable | Purpose |
|-----------|---------------------|---------|
| `shared/types/` | lib `shared` | Game types (`Ball`, `Paddle`, `Block`, `GameSnapshot`), constants |
| `shared/protocol/` | lib `shared` | Message structs, `sf::Packet` serialization |
| `game/` | lib `game_logic` | `GameState` (authoritative update loop), collision physics |
| `server/` | exe `arkanoid_server` | TCP accept thread, match lifecycle, leaderboard |
| `client/` | exe `arkanoid_client` | TCP client, SFML 2D renderer, input handling |
| `tests/` | test executables | Protocol round-trip and game-state unit tests |
| `docs/` | — | Protocol specification |

### Network protocol

TCP with `sf::Packet` (length-prefixed, endian-safe).

| Direction | Message | Purpose |
|-----------|---------|---------|
| C → S | `JoinRequest` | Join or reconnect (name + session token) |
| C → S | `PaddleInput` | Current paddle X position |
| C → S | `LeaveMatch` | Graceful disconnect |
| S → C | `JoinResponse` | Accept/reject with player ID and token |
| S → C | `WaitingForOpponent` | Waiting for second player |
| S → C | `MatchStarting` | Initial game snapshot |
| S → C | `StateUpdate` | Periodic game snapshot (30 Hz) |
| S → C | `MatchEnd` | Final scores and winner |
| S → C | `PlayerDisconnected` | Opponent dropped |
| S → C | `PlayerReconnected` | Opponent returned |

Full protocol details: [`docs/protocol.md`](docs/protocol.md)

### Reconnect

Each player receives a session token on join. If a client disconnects, the
match pauses for up to 30 seconds. Reconnecting with the same token restores
the player to their slot without duplication, and the match resumes.

### Leaderboard

Match results are appended to `leaderboard.txt` (pipe-delimited) with player
names, scores, winner, and timestamp. The server prints the leaderboard on
startup and after each match.

## Tech stack

- **C++17**, CMake 3.20+
- **SFML 2.6.1** — graphics, windowing, networking (fetched automatically via CMake FetchContent)
- No other external dependencies

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

On Windows, copy the SFML DLLs next to the executables:

```bash
copy build\_deps\sfml-build\lib\Release\*.dll build\server\Release\
copy build\_deps\sfml-build\lib\Release\*.dll build\client\Release\
```

### Run tests

```bash
cd build
ctest --build-config Release --output-on-failure
```

## Usage

### 1. Start the server

```bash
build/server/Release/arkanoid_server[.exe]
# or with a custom port:
build/server/Release/arkanoid_server 12345
```

### 2. Start two clients

```bash
# Terminal A
build/client/Release/arkanoid_client 127.0.0.1 54321 Alice

# Terminal B
build/client/Release/arkanoid_client 127.0.0.1 54321 Bob
```

Arguments: `<host> <port> <player_name> [session_token]`

The match starts automatically when both players connect.
Control your paddle with the mouse.

### 3. Test reconnect

1. Note the session token in the server log (e.g. `token=a1b2c3...`).
2. Close one client.
3. Server logs the disconnect; match pauses.
4. Relaunch the client with the token as the 4th argument:

```bash
build/client/Release/arkanoid_client 127.0.0.1 54321 Alice a1b2c3...
```

5. Server logs the reconnect; match resumes.

### 4. Leaderboard

After a match completes, results are saved to `leaderboard.txt` in the
server's working directory and printed to the console.

## Project structure

```
CMakeLists.txt              Root build (FetchContent for SFML)
shared/
  types/constants.h         Game constants
  types/game_types.h        Ball, Paddle, Block, GameSnapshot
  protocol/message_types.h  MessageType enum
  protocol/messages.h       Message structs + serialization
  protocol/messages.cpp     sf::Packet << >> implementations
game/
  game_state.h / .cpp       Authoritative game state
  physics.h / .cpp          Collision detection
server/
  server.h / .cpp           TCP server, accept thread, game loop
  match.h / .cpp            Match lifecycle, pause/resume
  leaderboard.h / .cpp      File-based score storage
  main.cpp                  Entry point
client/
  client_network.h / .cpp   TCP client, message polling
  renderer.h / .cpp         SFML 2D rendering
  main.cpp                  Entry point, input loop
tests/
  test_protocol.cpp         Protocol serialization round-trip
  test_game_state.cpp       Game state initialization and update
docs/
  protocol.md               Full protocol specification
```
