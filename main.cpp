#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "map.h"
#include "player.h"
#include "renderer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Raycaster", sf::Style::Close | sf::Style::Titlebar);

    // std::vector<std::vector<int>> grid {
    //     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    //     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    //     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    //     {1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1},
    //     {1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1},
    //     {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    //     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    // };

    Map map(12.0f, "maps/map4.png");

    Player player;
    player.position = sf::Vector2f(50, 50);

    Renderer renderer;
    renderer.init();
    
    sf::Clock gameClock;
    while (window.isOpen())
    {
        float deltaTime = gameClock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        player.update(deltaTime);


        window.clear();

        renderer.draw3Dview(window, player, map);

        map.draw(window);
        player.draw(window);
        renderer.drawRays(window, player, map);

        window.display();
    }
}