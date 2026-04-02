# Tao Arkanoid — Network Protocol

## Transport

TCP over `sf::TcpSocket` / `sf::Packet`.
Default port: **54321**.
Each message is a length-prefixed SFML packet with an initial `uint8_t` message type.

## Message Types

| Value | Name                 | Direction | Payload |
|-------|----------------------|-----------|---------|
| 1     | JoinRequest          | C → S     | `string playerName`, `string sessionToken` |
| 2     | PaddleInput          | C → S     | `float paddleX` |
| 3     | LeaveMatch           | C → S     | *(empty)* |
| 10    | JoinResponse         | S → C     | `bool accepted`, `uint8 playerId`, `string sessionToken`, opt `string rejectReason` |
| 11    | WaitingForOpponent   | S → C     | *(empty)* |
| 12    | MatchStarting        | S → C     | `GameSnapshot` |
| 13    | StateUpdate          | S → C     | `GameSnapshot` |
| 14    | MatchEnd             | S → C     | `uint8 winnerId`, `uint32 score1`, `uint32 score2` |
| 15    | PlayerDisconnected   | S → C     | `uint8 playerId` |
| 16    | PlayerReconnected    | S → C     | `uint8 playerId` |

## GameSnapshot Layout

| Field         | Type     |
|---------------|----------|
| ballX         | float    |
| ballY         | float    |
| ballVX        | float    |
| ballVY        | float    |
| ballActive    | bool     |
| paddle1X      | float    |
| paddle2X      | float    |
| score1        | uint32   |
| score2        | uint32   |
| state         | uint8    |
| yourPlayerId  | uint8    |
| blockCount    | uint16   |
| blocksAlive[] | uint8[]  |

## Reconnect Flow

1. Client stores `sessionToken` from JoinResponse.
2. On disconnect, client periodically retries `connect` + `JoinRequest{name, token}`.
3. Server matches token to an existing disconnected slot and reassigns the socket.
4. Server sends `JoinResponse` + latest `StateUpdate` to the reconnected client.
5. Server sends `PlayerReconnected` to the other client.
6. Match resumes from `Paused` → `Playing`.
