#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace tao {

struct LeaderboardEntry {
    std::string player1;
    std::string player2;
    uint32_t    score1   = 0;
    uint32_t    score2   = 0;
    std::string winner;
    std::string timestamp;
};

class Leaderboard {
public:
    void load(const std::string& path);
    void save(const std::string& path) const;

    void addResult(const std::string& p1, const std::string& p2,
                   uint32_t s1, uint32_t s2);

    const std::vector<LeaderboardEntry>& entries() const { return entries_; }
    void print() const;

private:
    std::vector<LeaderboardEntry> entries_;
};

} // namespace tao
