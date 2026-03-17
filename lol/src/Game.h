#pragma once
#include <SFML/Graphics.hpp>
#include "Bird.h"
#include "PipeManager.h"
#include "Leaderboard.h"
#include <vector>

enum class GameState { Menu, Playing, GameOver, LeaderboardView };

struct Cloud {
    sf::CircleShape parts[3];
    float x, y, speed;
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void renderBackground();
    void renderClouds();
    void renderGround();
    void renderScore();
    void renderMenu();
    void renderGameOver();
    void renderLeaderboard();

    void startGame();
    void endGame();
    void initClouds();
    void updateClouds(float dt);

    sf::RenderWindow m_window;
    GameState m_state;
    Bird m_bird;
    PipeManager m_pipes;
    Leaderboard m_lb;
    int m_score;
    sf::Font m_font;

    sf::VertexArray m_sky;
    sf::RectangleShape m_ground;
    sf::RectangleShape m_grass;
    std::vector<Cloud> m_clouds;

    float m_flashTimer;
    bool m_flashOn;

    static constexpr int W = 400;
    static constexpr int H = 700;
    static constexpr float GROUND_H = 60.f;
};
