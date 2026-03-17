#pragma once
#include <vector>
#include <string>

class Leaderboard {
public:
    explicit Leaderboard(const std::string& filename = "leaderboard.dat");
    void load();
    void save();
    void addScore(int score);
    const std::vector<int>& getScores() const;

private:
    std::vector<int> m_scores;
    std::string m_filename;
    static constexpr int MAX_ENTRIES = 10;
};
