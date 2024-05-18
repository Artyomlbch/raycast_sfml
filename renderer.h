#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "player.h"
#include "map.h"

constexpr const float SCREEN_WIDTH = 1280.0f;
constexpr const float SCREEN_HEIGHT = 720.0f;

class Renderer
{
public:
    void drawRays(sf::RenderTarget& target, const Player& player, const Map& map);
    void draw3Dview(sf::RenderTarget& target, const Player& player, const Map& map);

private:

};

#endif