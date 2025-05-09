#include <cmath>
#include <SFML/Graphics/RectangleShape.hpp>

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

static Ray castRay(sf::Vector2f start, float angleInDegrees, const Map& map);

void Renderer::drawRays(sf::RenderTarget& target, const Player& player, const Map& map)
{
    for (float angle = player.angle - player_FOV / 2; angle < player.angle + player_FOV / 2.0f; angle += 0.5f)
    {
        Ray ray = castRay(player.position, angle, map);

        if (ray.hit)
        {
            sf::Vertex line[] = {
            sf::Vertex(player.position),
            sf::Vertex(ray.hitPosition)
            };
            target.draw(line, 2, sf::Lines);
        }           
    }
}

Ray castRay(sf::Vector2f start, float angleInDegrees, const Map& map)
{
    float angle = angleInDegrees * PI / 180.0f;
    float vTan = -tan(angle), hTan = -1.0f / tan(angle);
    float cellSize = map.getCellSize();

    bool hit = false;
    size_t vdof = 0, hdof = 0;
    float vdist = std::numeric_limits<float>::max(), hdist = std::numeric_limits<float>::max();

    sf::Vector2u vMapPos, hMapPos;
    sf::Vector2f vrayPos, hrayPos, offset;

    if (cos(angle) > 0.001f) 
    {
        vrayPos.x = std::floor(start.x / cellSize) * cellSize + cellSize;
        vrayPos.y = (start.x - vrayPos.x) * vTan + start.y;

        offset.x = cellSize;
        offset.y = -offset.x * vTan;
    }
    else if (cos(angle) < -0.001f) 
    {
        vrayPos.x = std::floor(start.x / cellSize) * cellSize - 0.01f;
        vrayPos.y = (start.x - vrayPos.x) * vTan + start.y;

        offset.x = -cellSize;
        offset.y = -offset.x * vTan;
    }
    else { vdof = MAX_RAYCASTING_DEPTH; }

    const auto &grid = map.getGrid();
    for (; vdof < MAX_RAYCASTING_DEPTH; ++vdof)
    {
        int mapX = (int)(vrayPos.x / cellSize);
        int mapY = (int)(vrayPos.y / cellSize);

        if (mapY < grid.size() && mapX < grid[mapY].size() && grid[mapY][mapX] != sf::Color::Black)
        {
            hit = true;
            vdist = std::sqrt((vrayPos.x - start.x) * (vrayPos.x - start.x) + 
                              (vrayPos.y - start.y) * (vrayPos.y - start.y));
            vMapPos = sf::Vector2u(mapX, mapY);
            break;
        }

        vrayPos += offset;
    }

    if (sin(angle) > 0.001f) 
    {
        hrayPos.y = std::floor(start.y / cellSize) * cellSize + cellSize;
        hrayPos.x = (start.y - hrayPos.y) * hTan + start.x;

        offset.y = cellSize;
        offset.x = -offset.y * hTan;
    }
    else if (sin(angle) < -0.001f) 
    {
        hrayPos.y = std::floor(start.y / cellSize) * cellSize - 0.01f;
        hrayPos.x = (start.y - hrayPos.y) * hTan + start.x;

        offset.y = -cellSize;
        offset.x = -offset.y * hTan;
    }
    else { hdof = MAX_RAYCASTING_DEPTH;  }

    for (; hdof < MAX_RAYCASTING_DEPTH; ++hdof)
    {
        int mapX = (int)(hrayPos.x / cellSize);
        int mapY = (int)(hrayPos.y / cellSize);

        if (mapY < grid.size() && mapX < grid[mapY].size() && grid[mapY][mapX] != sf::Color::Black)
        {
            hit = true;
            hdist = std::sqrt((hrayPos.x - start.x) * (hrayPos.x - start.x) + 
                              (hrayPos.y - start.y) * (hrayPos.y - start.y));
            hMapPos = sf::Vector2u(mapX, mapY);
            break;
        }

        hrayPos += offset;
    }

    return Ray{hdist < vdist ? hrayPos : vrayPos, hdist < vdist ? hMapPos : vMapPos, std::min(hdist, vdist), hit, vdist <= hdist};
}

void Renderer::draw3Dview(sf::RenderTarget& target, const Player& player, const Map& map)
{
    // Drawing sky (just rectangle at half of the screen)
    sf::RectangleShape sky_floor(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2.0f));
    sky_floor.setFillColor(sf::Color(100, 170, 250));
    target.draw(sky_floor);

    // Drawing floor
    sky_floor.setPosition(0.0f, SCREEN_HEIGHT / 2.0f);
    sky_floor.setFillColor(sf::Color(70, 70, 70));
    target.draw(sky_floor);

    // Setting angles and DOF
    float angle = player.angle - player_FOV / 2.0f;
    const float angleIncrement = player_FOV / (float)NUM_RAYS;
    float maxRenderDistance = MAX_RAYCASTING_DEPTH * map.getCellSize();

    for (size_t i = 0; i < NUM_RAYS; ++i, angle += angleIncrement)
    {
        Ray ray = castRay(player.position, angle, map);

        if (ray.hit)
        {
            ray.distance *= cos((player.angle - angle) * PI / 180.0f);  // removing "fish eye"

            float wallHeight = (map.getCellSize() * SCREEN_HEIGHT) / ray.distance;
            if (wallHeight > SCREEN_HEIGHT)
            {
                wallHeight = SCREEN_HEIGHT;
            }

            float brightness = 1.0f - (ray.distance / maxRenderDistance);
            if (brightness < 0.0f) brightness = 0.0f;
            float shade = (ray.isHitVertical ? 0.8f : 1.0f) * brightness; 

            float wallOffset = SCREEN_HEIGHT / 2.0f - wallHeight / 2.0f;

            sf::RectangleShape column(sf::Vector2f(COLUMN_WIDTH, wallHeight));
            column.setPosition(i * COLUMN_WIDTH, wallOffset);

            sf::Color color = map.getGrid()[ray.mapPosition.y][ray.mapPosition.x];

            column.setFillColor(sf::Color(color.r * shade, color.g * shade, color.b * shade));
            target.draw(column);
        }           
    }
}
