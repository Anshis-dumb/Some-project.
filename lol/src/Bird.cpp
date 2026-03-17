#include "Bird.h"
#include <cmath>

Bird::Bird() : m_velocity(0.f), m_rotation(0.f), m_position{X_POS, 300.f} {
    m_body.setRadius(RADIUS);
    m_body.setOrigin(RADIUS, RADIUS);
    m_body.setFillColor(sf::Color(255, 204, 0));
    m_body.setOutlineColor(sf::Color(204, 153, 0));
    m_body.setOutlineThickness(1.5f);

    m_wing.setRadius(8.f);
    m_wing.setOrigin(8.f, 8.f);
    m_wing.setFillColor(sf::Color(255, 170, 0));

    m_eye.setRadius(4.5f);
    m_eye.setOrigin(4.5f, 4.5f);
    m_eye.setFillColor(sf::Color::White);
    m_eye.setOutlineColor(sf::Color(50, 50, 50));
    m_eye.setOutlineThickness(0.5f);

    m_pupil.setRadius(2.f);
    m_pupil.setOrigin(2.f, 2.f);
    m_pupil.setFillColor(sf::Color::Black);

    m_beak.setPointCount(3);
    m_beak.setFillColor(sf::Color(255, 100, 50));

    updateShapes();
}

void Bird::reset(float windowHeight) {
    m_velocity = 0.f;
    m_rotation = 0.f;
    m_position = {X_POS, windowHeight / 2.f};
    updateShapes();
}

void Bird::jump() {
    m_velocity = JUMP_VELOCITY;
}

void Bird::update(float dt) {
    m_velocity += GRAVITY * dt;
    if (m_velocity > MAX_VELOCITY) m_velocity = MAX_VELOCITY;

    m_position.y += m_velocity * dt;

    float target = m_velocity * 0.12f;
    if (target > 60.f) target = 60.f;
    if (target < -30.f) target = -30.f;
    m_rotation += (target - m_rotation) * 8.f * dt;

    updateShapes();
}

void Bird::updateShapes() {
    m_body.setPosition(m_position);
    m_body.setRotation(m_rotation);
    m_wing.setPosition(m_position.x - 6.f, m_position.y + 2.f);
    m_eye.setPosition(m_position.x + 6.f, m_position.y - 5.f);
    m_pupil.setPosition(m_position.x + 7.5f, m_position.y - 5.f);
    m_beak.setPoint(0, sf::Vector2f(m_position.x + RADIUS - 2.f, m_position.y - 2.f));
    m_beak.setPoint(1, sf::Vector2f(m_position.x + RADIUS + 9.f, m_position.y + 3.f));
    m_beak.setPoint(2, sf::Vector2f(m_position.x + RADIUS - 2.f, m_position.y + 6.f));
}

void Bird::render(sf::RenderWindow& window) {
    window.draw(m_wing);
    window.draw(m_body);
    window.draw(m_beak);
    window.draw(m_eye);
    window.draw(m_pupil);
}

sf::FloatRect Bird::getBounds() const {
    return sf::FloatRect(
        m_position.x - RADIUS + 3.f,
        m_position.y - RADIUS + 3.f,
        (RADIUS - 3.f) * 2.f,
        (RADIUS - 3.f) * 2.f
    );
}
