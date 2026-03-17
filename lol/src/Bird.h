#pragma once
#include <SFML/Graphics.hpp>

class Bird {
public:
    Bird();
    void reset(float windowHeight);
    void jump();
    void update(float dt);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    float getY() const { return m_position.y; }

private:
    void updateShapes();

    sf::Vector2f m_position;
    float m_velocity;
    float m_rotation;

    sf::CircleShape m_body;
    sf::CircleShape m_wing;
    sf::CircleShape m_eye;
    sf::CircleShape m_pupil;
    sf::ConvexShape m_beak;

    static constexpr float RADIUS = 15.f;
    static constexpr float X_POS = 80.f;
    static constexpr float GRAVITY = 800.f;
    static constexpr float JUMP_VELOCITY = -330.f;
    static constexpr float MAX_VELOCITY = 500.f;
};
