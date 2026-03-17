#include "PipeManager.h"
#include <algorithm>
#include <cstdlib>

PipeManager::PipeManager() : m_spawnTimer(0.f) {}

void PipeManager::reset() {
    m_pipes.clear();
    m_spawnTimer = 0.f;
}

void PipeManager::spawnPipe(float windowWidth, float windowHeight, float groundHeight) {
    Pipe p;
    p.x = windowWidth;
    p.scored = false;

    float minY = GAP_SIZE / 2.f + 60.f;
    float maxY = windowHeight - groundHeight - GAP_SIZE / 2.f - 60.f;
    p.gapCenterY = minY + static_cast<float>(std::rand()) / RAND_MAX * (maxY - minY);

    float topH = p.gapCenterY - GAP_SIZE / 2.f;
    float botY = p.gapCenterY + GAP_SIZE / 2.f;
    float botH = windowHeight - groundHeight - botY;

    // Top pipe body
    p.topPipe.setSize({PIPE_WIDTH, topH});
    p.topPipe.setFillColor(sf::Color(76, 175, 80));
    p.topPipe.setOutlineColor(sf::Color(56, 142, 60));
    p.topPipe.setOutlineThickness(2.f);

    // Top cap
    p.topCap.setSize({PIPE_WIDTH + CAP_EXTRA * 2, CAP_HEIGHT});
    p.topCap.setFillColor(sf::Color(56, 142, 60));
    p.topCap.setOutlineColor(sf::Color(46, 125, 50));
    p.topCap.setOutlineThickness(2.f);

    // Bottom pipe body
    p.bottomPipe.setSize({PIPE_WIDTH, botH});
    p.bottomPipe.setFillColor(sf::Color(76, 175, 80));
    p.bottomPipe.setOutlineColor(sf::Color(56, 142, 60));
    p.bottomPipe.setOutlineThickness(2.f);

    // Bottom cap
    p.bottomCap.setSize({PIPE_WIDTH + CAP_EXTRA * 2, CAP_HEIGHT});
    p.bottomCap.setFillColor(sf::Color(56, 142, 60));
    p.bottomCap.setOutlineColor(sf::Color(46, 125, 50));
    p.bottomCap.setOutlineThickness(2.f);

    // Set initial positions
    p.topPipe.setPosition(p.x, 0.f);
    p.topCap.setPosition(p.x - CAP_EXTRA, topH - CAP_HEIGHT);
    p.bottomPipe.setPosition(p.x, botY);
    p.bottomCap.setPosition(p.x - CAP_EXTRA, botY);

    m_pipes.push_back(p);
}

void PipeManager::update(float dt, float windowWidth, float windowHeight, float groundHeight) {
    m_spawnTimer += dt;
    if (m_spawnTimer >= SPAWN_INTERVAL) {
        m_spawnTimer -= SPAWN_INTERVAL;
        spawnPipe(windowWidth, windowHeight, groundHeight);
    }

    for (auto& p : m_pipes) {
        p.x -= SPEED * dt;
        float topH = p.gapCenterY - GAP_SIZE / 2.f;
        float botY = p.gapCenterY + GAP_SIZE / 2.f;
        p.topPipe.setPosition(p.x, 0.f);
        p.topCap.setPosition(p.x - CAP_EXTRA, topH - CAP_HEIGHT);
        p.bottomPipe.setPosition(p.x, botY);
        p.bottomCap.setPosition(p.x - CAP_EXTRA, botY);
    }

    float threshold = -(PIPE_WIDTH + CAP_EXTRA * 2);
    m_pipes.erase(
        std::remove_if(m_pipes.begin(), m_pipes.end(),
            [threshold](const Pipe& p) { return p.x < threshold; }),
        m_pipes.end()
    );
}

void PipeManager::render(sf::RenderWindow& window) {
    for (auto& p : m_pipes) {
        window.draw(p.topPipe);
        window.draw(p.topCap);
        window.draw(p.bottomPipe);
        window.draw(p.bottomCap);
    }
}

std::vector<Pipe>& PipeManager::getPipes() { return m_pipes; }
