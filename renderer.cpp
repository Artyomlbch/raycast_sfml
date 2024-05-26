#include <cmath>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>

#include "renderer.h"

constexpr float PI = 3.141592653589793f;
constexpr size_t MAX_RAYCASTING_DEPTH = 32;
constexpr float player_FOV = 60.0f;
constexpr size_t NUM_RAYS = 600;
constexpr float COLUMN_WIDTH = SCREEN_WIDTH / (float)NUM_RAYS;

struct Ray
{
    sf::Vector2f hitPosition;
    sf::Vector2u mapPosition;
    float distance;
    bool hit;
    bool isHitVertical;
};

void Renderer::init()
{
    if (!wallTexture.loadFromFile("textures/wallTexture_12x12.png"))
    {
        std::cerr << "ERROR: Failed to load Wall Texture." << std::endl; 
        return;
    }

    if (wallTexture.getSize().x != wallTexture.getSize().y)
    {
        std::cerr << "ERROR: Texture is not square shape." << std::endl;
        return;
    }

    wallSprite = sf::Sprite(wallTexture);
}

// void Renderer::drawRays(sf::RenderTarget& target, const Player& player, const Map& map)
// {
//     for (float angle = player.angle - player_FOV / 2; angle < player.angle + player_FOV / 2.0f; angle += 0.5f)
//     {
//         Ray ray = castRay(player.position, angle, map);

//         if (ray.hit)
//         {
//             sf::Vertex line[] = {
//             sf::Vertex(player.position),
//             sf::Vertex(ray.hitPosition)
//             };
//             target.draw(line, 2, sf::Lines);
//         }           
//     }
// }

void Renderer::draw3Dview(sf::RenderTarget& target, const Player& player, const Map& map)
{
    sf::Color floor_color = map.getGrid()[1][1];

    // Drawing sky (just rectangle at half of the screen)
    sf::RectangleShape sky_floor(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2.0f));
    sky_floor.setFillColor(sf::Color(135, 206, 235));
    target.draw(sky_floor);

    const sf::Color fogColor = sf::Color(100, 206, 235);

    // Drawing floor
    sky_floor.setPosition(0.0f, SCREEN_HEIGHT / 2.0f);
    sky_floor.setFillColor(floor_color);
    target.draw(sky_floor);

    float radians = player.angle * PI / 180.0f;
    sf::Vector2f direction{std::cos(radians), std::sin(radians)};
    sf::Vector2f plane{-direction.y, direction.x};

    sf::VertexArray walls{sf::Lines};

    for (size_t i = 0; i < SCREEN_WIDTH; ++i)
    {
        float cameraX = i * 2.0f / SCREEN_WIDTH - 1.0f; // -1.0f -> 0.0f -> 1.0f
        sf::Vector2f rayPos = player.position / map.getCellSize();
        sf::Vector2f rayDir = direction + plane * cameraX;

        sf::Vector2f deltaDist{std::abs(1.0f / rayDir.x), std::abs(1.0f / rayDir.y)};

        sf::Vector2i mapPos{rayPos};
        sf::Vector2i step;
        sf::Vector2f sideDist;

        if (rayDir.x < 0.0f)
        {
            step.x = -1;
            sideDist.x = (-mapPos.x + rayPos.x) * deltaDist.x;
        }
        else
        {
            step.x = 1;
            sideDist.x = (mapPos.x - rayPos.x + 1.0f) * deltaDist.x;
        }

        if (rayDir.y < 0.0f)
        {
            step.y = -1;
            sideDist.y = (-mapPos.y + rayPos.y) * deltaDist.y;
        }
        else
        {
            step.y = 1;
            sideDist.y = (mapPos.y - rayPos.y + 1.0f) * deltaDist.y;
        }

        bool didHit{}, isHitVertical{};

        size_t depth = 0;
        while (!didHit && depth < MAX_RAYCASTING_DEPTH)
        {
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += deltaDist.x;
                mapPos.x += step.x;
                isHitVertical = false;
            }
            else
            {
                sideDist.y += deltaDist.y;
                mapPos.y += step.y;
                isHitVertical = true;
            }

            int x = mapPos.x, y = mapPos.y;
            const auto &grid = map.getGrid();

            if (y >= 0 && y < grid.size() && x >= 0 && x < grid[y].size() && grid[y][x] != floor_color)
            {
                didHit = true;
            }

            depth++;
        }

        if (didHit)
        {
            float perpWallDist = isHitVertical ? sideDist.y - deltaDist.y 
                                               : sideDist.x - deltaDist.x;

            float wallHeight = SCREEN_HEIGHT / perpWallDist;

            float wallStart = (-wallHeight + SCREEN_HEIGHT) / 2.0f;
            float wallEnd = (wallHeight + SCREEN_HEIGHT) / 2.0f;

            float textureSize = wallTexture.getSize().x;

            float wallX = isHitVertical ? rayPos.x + perpWallDist * rayDir.x 
                                        : rayPos.y + perpWallDist * rayDir.y;

            wallX -= floor(wallX);
            float textureX = wallX * textureSize;

            float brightness = 1.0f - (perpWallDist / (float)MAX_RAYCASTING_DEPTH);
            sf::Color color = sf::Color(255 * brightness, 255 * brightness, 255 * brightness);
            
            if (isHitVertical)
            {
                brightness *= 0.7f;
            }

            walls.append(sf::Vertex(sf::Vector2f((float)i, wallStart), color, sf::Vector2f(textureX, 0.0f)));
            walls.append(sf::Vertex(sf::Vector2f((float)i, wallEnd), color, sf::Vector2f(textureX, textureSize)));    
        }
    }

    sf::RenderStates states{&wallTexture};
    target.draw(walls, states);
}
