#include "Leaderboard.h"
#include <fstream>
#include <algorithm>
#include <functional>

Leaderboard::Leaderboard(const std::string& filename) : m_filename(filename) {}

void Leaderboard::load() {
    m_scores.clear();
    std::ifstream file(m_filename);
    if (!file.is_open()) return;
    int score;
    while (file >> score) {
        m_scores.push_back(score);
    }
    std::sort(m_scores.begin(), m_scores.end(), std::greater<int>());
    if (m_scores.size() > static_cast<size_t>(MAX_ENTRIES))
        m_scores.resize(MAX_ENTRIES);
}

void Leaderboard::save() {
    std::ofstream file(m_filename);
    for (int s : m_scores)
        file << s << "\n";
}

void Leaderboard::addScore(int score) {
    m_scores.push_back(score);
    std::sort(m_scores.begin(), m_scores.end(), std::greater<int>());
    if (m_scores.size() > static_cast<size_t>(MAX_ENTRIES))
        m_scores.resize(MAX_ENTRIES);
    save();
}

const std::vector<int>& Leaderboard::getScores() const { return m_scores; }
