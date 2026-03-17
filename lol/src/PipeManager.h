#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Pipe {
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    sf::RectangleShape topCap;
    sf::RectangleShape bottomCap;
    float x;
    float gapCenterY;
    bool scored;
};

class PipeManager {
public:
    PipeManager();
    void reset();
    void update(float dt, float windowWidth, float windowHeight, float groundHeight);
    void render(sf::RenderWindow& window);
    std::vector<Pipe>& getPipes();

private:
    std::vector<Pipe> m_pipes;
    float m_spawnTimer;

    void spawnPipe(float windowWidth, float windowHeight, float groundHeight);

    static constexpr float PIPE_WIDTH = 55.f;
    static constexpr float GAP_SIZE = 150.f;
    static constexpr float SPEED = 180.f;
    static constexpr float SPAWN_INTERVAL = 1.8f;
    static constexpr float CAP_HEIGHT = 20.f;
    static constexpr float CAP_EXTRA = 6.f;
};
