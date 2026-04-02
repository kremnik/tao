#pragma once

#include <SFML/Network.hpp>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

namespace tao {

class Match;
class Leaderboard;
struct JoinRequest;

struct PlayerSlot {
    std::unique_ptr<sf::TcpSocket> socket;
    std::string name;
    std::string sessionToken;
    bool connected = false;
    bool assigned  = false;
};

class Server {
public:
    Server();
    ~Server();

    bool start(uint16_t port);
    void run();
    void stop();

private:
    void acceptLoop();
    void collectNewConnections();
    void handlePending();
    void handlePlayers();
    void tryStartMatch();
    void updateMatch(float dt);
    void broadcastState();
    void finishMatch();
    void handleJoin(std::unique_ptr<sf::TcpSocket> sock,
                    const JoinRequest& req);
    void removePlayer(int slot);

    std::string generateToken();
    int  findSlotByToken(const std::string& token);

    sf::TcpListener listener_;

    std::thread acceptThread_;
    std::mutex  newConnMutex_;
    std::vector<std::unique_ptr<sf::TcpSocket>> newConnections_;

    struct PendingConn {
        std::unique_ptr<sf::TcpSocket> socket;
    };
    std::vector<PendingConn> pending_;

    PlayerSlot players_[2];

    std::unique_ptr<Match>       match_;
    std::unique_ptr<Leaderboard> leaderboard_;

    std::atomic<bool> running_{false};
    sf::Clock clock_;
    float    stateAccumulator_ = 0.0f;
};

} // namespace tao
