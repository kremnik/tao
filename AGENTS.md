# AGENTS.md

## Project
Networked 2-player Arkanoid in C++:
- authoritative server
- two clients
- shared protocol (SFML sf::Packet over TCP)
- reconnect support (session tokens)
- leaderboard (file-based)
- CMake build with SFML 2.6 via FetchContent

## Tech stack
- C++17, CMake 3.20+
- SFML 2.6.1 (graphics, window, network, system) fetched via FetchContent
- Default port: 54321

## Architecture rules
- Server is the source of truth for game state.
- Clients send input and render received state.
- Keep rendering, networking, and game logic separated.
- Store protocol definitions in shared code.
- Do not mix client-only and server-only responsibilities.

## Module layout
- `shared/` — protocol messages, game types, constants (lib: shared)
- `game/` — game state, physics (lib: game_logic)
- `server/` — server networking, match management, leaderboard (exe: arkanoid_server)
- `client/` — client networking, SFML renderer (exe: arkanoid_client)
- `tests/` — unit tests for protocol and game state
- `docs/` — protocol specification

## Coding rules
- Prefer small, local changes over large rewrites.
- Keep headers clean; avoid cyclic dependencies.
- Use modern C++ where it improves clarity and safety.
- Avoid hidden global state.
- Keep gameplay constants centralized in shared/types/constants.h.

## Verification
After changes, if possible:
- build the project;
- run tests (`ctest --build-config Release`);
- verify server and client both compile;
- for networking changes, check reconnect scenario;
- for protocol changes, update all send/receive paths.

## Done criteria
A task is done only if:
- the code builds;
- the feature works;
- existing behavior is not obviously broken;
- the final note explains what changed and how it was verified.