# AGENTS.md

## Project
Networked 2-player Arkanoid in C++:
- authoritative server
- two clients
- shared protocol
- reconnect support
- leaderboard
- CMake build

## Architecture rules
- Server is the source of truth for game state.
- Clients send input and render received state.
- Keep rendering, networking, and game logic separated.
- Store protocol definitions in shared code.
- Do not mix client-only and server-only responsibilities.

## Coding rules
- Prefer small, local changes over large rewrites.
- Keep headers clean; avoid cyclic dependencies.
- Use modern C++ where it improves clarity and safety.
- Avoid hidden global state.
- Keep gameplay constants centralized.

## Verification
After changes, if possible:
- build the project;
- run tests;
- verify server and client both compile;
- for networking changes, check reconnect scenario;
- for protocol changes, update all send/receive paths.

## Done criteria
A task is done only if:
- the code builds;
- the feature works;
- existing behavior is not obviously broken;
- the final note explains what changed and how it was verified.