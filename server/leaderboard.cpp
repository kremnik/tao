#include "leaderboard.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>

namespace tao {

void Leaderboard::load(const std::string& path) {
    entries_.clear();
    std::ifstream in(path);
    if (!in.is_open()) return;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        LeaderboardEntry e;
        char sep;
        if (std::getline(ss, e.player1, '|') &&
            (ss >> e.score1) && (ss >> sep) &&
            std::getline(ss, e.player2, '|') &&
            (ss >> e.score2) && (ss >> sep) &&
            std::getline(ss, e.winner, '|') &&
            std::getline(ss, e.timestamp)) {
            entries_.push_back(std::move(e));
        }
    }
}

void Leaderboard::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "Cannot write leaderboard to " << path << std::endl;
        return;
    }
    out << "# player1|score1|player2|score2|winner|timestamp\n";
    for (const auto& e : entries_) {
        out << e.player1 << '|' << e.score1 << '|'
            << e.player2 << '|' << e.score2 << '|'
            << e.winner  << '|' << e.timestamp << '\n';
    }
}

void Leaderboard::addResult(const std::string& p1, const std::string& p2,
                            uint32_t s1, uint32_t s2) {
    LeaderboardEntry e;
    e.player1 = p1;
    e.player2 = p2;
    e.score1  = s1;
    e.score2  = s2;
    e.winner  = (s1 > s2) ? p1 : (s2 > s1) ? p2 : "Draw";

    auto t = std::time(nullptr);
    std::ostringstream ts;
    ts << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    e.timestamp = ts.str();

    entries_.push_back(std::move(e));
}

void Leaderboard::print() const {
    std::cout << "=== Leaderboard ===\n";
    for (const auto& e : entries_) {
        std::cout << e.player1 << " " << e.score1 << " vs "
                  << e.player2 << " " << e.score2
                  << "  winner: " << e.winner
                  << "  (" << e.timestamp << ")\n";
    }
    if (entries_.empty()) std::cout << "(empty)\n";
    std::cout << "===================\n";
}

} // namespace tao
