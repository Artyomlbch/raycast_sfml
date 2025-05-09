#include "player.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include<SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

constexpr float PI = 3.141592653589793f;
constexpr float TURN_SPEED = 80.0f;
constexpr float MOVE_SPEED = 30.0f;

void Player::draw(sf::RenderTarget& target)
{
    sf::CircleShape circle(8.0f);
    circle.setOrigin(circle.getRadius(), circle.getRadius());

    circle.setPosition(position);
    circle.setFillColor(sf::Color::Blue);

    sf::RectangleShape line(sf::Vector2f(24.0f, 2.0f));
    line.setPosition(position);
    line.setRotation(angle);
    line.setFillColor(sf::Color::Blue);

    target.draw(circle);
    target.draw(line);
}

void Player::update(float deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        angle -= TURN_SPEED * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        angle += TURN_SPEED * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        float radians = angle * PI / 180.0f;

        position.x += cos(radians) * MOVE_SPEED * deltaTime;
        position.y += sin(radians) * MOVE_SPEED * deltaTime;
    }
}