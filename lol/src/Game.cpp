#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cmath>

namespace {
    void centerText(sf::Text& t, float x, float y) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(x, y);
    }
}

Game::Game()
    : m_window(sf::VideoMode(W, H), "Flappy Bird", sf::Style::Close)
    , m_state(GameState::Menu), m_score(0)
    , m_flashTimer(0.f), m_flashOn(true)
    , m_sky(sf::Quads, 4)
{
    m_window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Try loading a font
    if (!m_font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
        m_font.loadFromFile("C:\\Windows\\Fonts\\segoeui.ttf");

    // Sky gradient
    sf::Color top(135, 206, 235), bot(70, 130, 180);
    m_sky[0] = {{0,0}, top};
    m_sky[1] = {{(float)W,0}, top};
    m_sky[2] = {{(float)W,(float)H}, bot};
    m_sky[3] = {{0,(float)H}, bot};

    // Ground
    m_ground.setSize({(float)W, GROUND_H});
    m_ground.setPosition(0, H - GROUND_H);
    m_ground.setFillColor(sf::Color(194, 163, 107));

    m_grass.setSize({(float)W, 10.f});
    m_grass.setPosition(0, H - GROUND_H);
    m_grass.setFillColor(sf::Color(76, 175, 80));

    initClouds();
    m_lb.load();
}

void Game::initClouds() {
    m_clouds.clear();
    for (int i = 0; i < 5; ++i) {
        Cloud c;
        c.x = static_cast<float>(std::rand() % W);
        c.y = 30.f + static_cast<float>(std::rand() % 200);
        c.speed = 15.f + static_cast<float>(std::rand() % 20);
        float r = 18.f + static_cast<float>(std::rand() % 12);
        for (int j = 0; j < 3; ++j) {
            c.parts[j].setRadius(r - j * 3.f);
            c.parts[j].setFillColor(sf::Color(255, 255, 255, 180));
            c.parts[j].setOrigin(r - j * 3.f, r - j * 3.f);
        }
        m_clouds.push_back(c);
    }
}

void Game::updateClouds(float dt) {
    for (auto& c : m_clouds) {
        c.x += c.speed * dt;
        if (c.x > W + 60) c.x = -60.f;
        c.parts[0].setPosition(c.x, c.y);
        c.parts[1].setPosition(c.x + 20.f, c.y - 5.f);
        c.parts[2].setPosition(c.x + 38.f, c.y + 2.f);
    }
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event ev;
    while (m_window.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed)
            m_window.close();

        if (ev.type == sf::Event::KeyPressed) {
            switch (m_state) {
            case GameState::Menu:
                if (ev.key.code == sf::Keyboard::Space) startGame();
                if (ev.key.code == sf::Keyboard::L) m_state = GameState::LeaderboardView;
                break;
            case GameState::Playing:
                if (ev.key.code == sf::Keyboard::Space) m_bird.jump();
                break;
            case GameState::GameOver:
                if (ev.key.code == sf::Keyboard::Space) startGame();
                if (ev.key.code == sf::Keyboard::M) m_state = GameState::Menu;
                break;
            case GameState::LeaderboardView:
                if (ev.key.code == sf::Keyboard::M) m_state = GameState::Menu;
                break;
            }
        }
        if (ev.type == sf::Event::MouseButtonPressed) {
            if (m_state == GameState::Playing) m_bird.jump();
            else if (m_state == GameState::Menu) startGame();
        }
    }
}

void Game::update(float dt) {
    updateClouds(dt);
    m_flashTimer += dt;
    if (m_flashTimer >= 0.5f) { m_flashTimer -= 0.5f; m_flashOn = !m_flashOn; }

    if (m_state != GameState::Playing) return;

    m_bird.update(dt);
    m_pipes.update(dt, (float)W, (float)H, GROUND_H);

    sf::FloatRect bb = m_bird.getBounds();

    // Ground / ceiling
    if (bb.top + bb.height >= H - GROUND_H || bb.top <= 0) { endGame(); return; }

    // Pipes
    for (auto& p : m_pipes.getPipes()) {
        if (bb.intersects(p.topPipe.getGlobalBounds()) ||
            bb.intersects(p.bottomPipe.getGlobalBounds()) ||
            bb.intersects(p.topCap.getGlobalBounds()) ||
            bb.intersects(p.bottomCap.getGlobalBounds())) {
            endGame(); return;
        }
        float pr = p.topPipe.getGlobalBounds().left + p.topPipe.getGlobalBounds().width;
        if (!p.scored && pr < bb.left) { p.scored = true; m_score++; }
    }
}

// ---- Rendering -----------------------------------------------------------

void Game::render() {
    m_window.clear();
    renderBackground();
    renderClouds();

    if (m_state == GameState::Playing || m_state == GameState::GameOver)
        m_pipes.render(m_window);

    renderGround();

    if (m_state == GameState::Playing || m_state == GameState::GameOver) {
        m_bird.render(m_window);
        renderScore();
    }

    switch (m_state) {
    case GameState::Menu:            renderMenu();        break;
    case GameState::GameOver:        renderGameOver();    break;
    case GameState::LeaderboardView: renderLeaderboard(); break;
    default: break;
    }
    m_window.display();
}

void Game::renderBackground() { m_window.draw(m_sky); }
void Game::renderGround()     { m_window.draw(m_ground); m_window.draw(m_grass); }
void Game::renderClouds() {
    for (auto& c : m_clouds)
        for (int i = 0; i < 3; ++i) m_window.draw(c.parts[i]);
}

void Game::renderScore() {
    sf::Text t;
    t.setFont(m_font);
    t.setString(std::to_string(m_score));
    t.setCharacterSize(48);
    t.setFillColor(sf::Color::White);
    t.setOutlineColor(sf::Color(0,0,0,160));
    t.setOutlineThickness(3.f);
    t.setStyle(sf::Text::Bold);
    centerText(t, W / 2.f, 60.f);
    m_window.draw(t);
}

void Game::renderMenu() {
    sf::Text title;
    title.setFont(m_font);
    title.setString("FLAPPY BIRD");
    title.setCharacterSize(46);
    title.setFillColor(sf::Color::White);
    title.setOutlineColor(sf::Color(0,0,0,150));
    title.setOutlineThickness(3.f);
    title.setStyle(sf::Text::Bold);
    centerText(title, W/2.f, 200.f);
    m_window.draw(title);

    if (m_flashOn) {
        sf::Text sub;
        sub.setFont(m_font);
        sub.setString("Press SPACE to Play");
        sub.setCharacterSize(22);
        sub.setFillColor(sf::Color(255,255,255,220));
        sub.setOutlineColor(sf::Color(0,0,0,80));
        sub.setOutlineThickness(1.f);
        centerText(sub, W/2.f, 360.f);
        m_window.draw(sub);
    }

    sf::Text lb;
    lb.setFont(m_font);
    lb.setString("Press L for Leaderboard");
    lb.setCharacterSize(16);
    lb.setFillColor(sf::Color(255,255,255,180));
    centerText(lb, W/2.f, 420.f);
    m_window.draw(lb);
}

void Game::renderGameOver() {
    // Overlay
    sf::RectangleShape ov({(float)W,(float)H});
    ov.setFillColor(sf::Color(0,0,0,150));
    m_window.draw(ov);

    // Panel
    sf::RectangleShape panel({280, 300});
    panel.setOrigin(140, 150);
    panel.setPosition(W/2.f, H/2.f - 20.f);
    panel.setFillColor(sf::Color(44, 62, 80, 230));
    panel.setOutlineColor(sf::Color(255,204,0));
    panel.setOutlineThickness(3.f);
    m_window.draw(panel);

    sf::Text go;
    go.setFont(m_font); go.setString("GAME OVER");
    go.setCharacterSize(36); go.setFillColor(sf::Color(231,76,60));
    go.setStyle(sf::Text::Bold); centerText(go, W/2.f, H/2.f - 120.f);
    m_window.draw(go);

    sf::Text sc;
    sc.setFont(m_font); sc.setString("Score: " + std::to_string(m_score));
    sc.setCharacterSize(28); sc.setFillColor(sf::Color::White);
    centerText(sc, W/2.f, H/2.f - 55.f);
    m_window.draw(sc);

    int best = m_lb.getScores().empty() ? 0 : m_lb.getScores()[0];
    sf::Text bs;
    bs.setFont(m_font); bs.setString("Best: " + std::to_string(best));
    bs.setCharacterSize(22); bs.setFillColor(sf::Color(255,204,0));
    centerText(bs, W/2.f, H/2.f - 10.f);
    m_window.draw(bs);

    if (m_flashOn) {
        sf::Text r;
        r.setFont(m_font); r.setString("Press SPACE to Retry");
        r.setCharacterSize(18); r.setFillColor(sf::Color(255,255,255,200));
        centerText(r, W/2.f, H/2.f + 55.f);
        m_window.draw(r);
    }

    sf::Text m;
    m.setFont(m_font); m.setString("Press M for Menu");
    m.setCharacterSize(16); m.setFillColor(sf::Color(255,255,255,150));
    centerText(m, W/2.f, H/2.f + 100.f);
    m_window.draw(m);
}

void Game::renderLeaderboard() {
    sf::RectangleShape ov({(float)W,(float)H});
    ov.setFillColor(sf::Color(0,0,0,170));
    m_window.draw(ov);

    sf::RectangleShape panel({320, 460});
    panel.setOrigin(160, 230);
    panel.setPosition(W/2.f, H/2.f);
    panel.setFillColor(sf::Color(44, 62, 80, 240));
    panel.setOutlineColor(sf::Color(255,204,0));
    panel.setOutlineThickness(3.f);
    m_window.draw(panel);

    sf::Text title;
    title.setFont(m_font); title.setString("LEADERBOARD");
    title.setCharacterSize(30); title.setFillColor(sf::Color(255,204,0));
    title.setStyle(sf::Text::Bold);
    centerText(title, W/2.f, H/2.f - 190.f);
    m_window.draw(title);

    auto& scores = m_lb.getScores();
    for (size_t i = 0; i < scores.size() && i < 10; ++i) {
        sf::Text entry;
        entry.setFont(m_font);
        entry.setString(std::to_string(i+1) + ".  " + std::to_string(scores[i]));
        entry.setCharacterSize(20);
        entry.setFillColor(i == 0 ? sf::Color(255,215,0) :
                           i == 1 ? sf::Color(192,192,192) :
                           i == 2 ? sf::Color(205,127,50) :
                                    sf::Color::White);
        centerText(entry, W/2.f, H/2.f - 140.f + i * 36.f);
        m_window.draw(entry);
    }

    if (scores.empty()) {
        sf::Text nope;
        nope.setFont(m_font); nope.setString("No scores yet!");
        nope.setCharacterSize(18); nope.setFillColor(sf::Color(200,200,200));
        centerText(nope, W/2.f, H/2.f);
        m_window.draw(nope);
    }

    sf::Text back;
    back.setFont(m_font); back.setString("Press M for Menu");
    back.setCharacterSize(16); back.setFillColor(sf::Color(255,255,255,150));
    centerText(back, W/2.f, H/2.f + 220.f);
    m_window.draw(back);
}

// ---- State transitions ---------------------------------------------------

void Game::startGame() {
    m_state = GameState::Playing;
    m_score = 0;
    m_bird.reset((float)H);
    m_pipes.reset();
}

void Game::endGame() {
    m_state = GameState::GameOver;
    m_lb.addScore(m_score);
}
