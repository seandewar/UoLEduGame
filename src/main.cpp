#include <cstdlib>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Game.h"


int main(int argc, char* argv[])
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "UoLEduGame");

    const auto targetFrameRate = static_cast<unsigned int>(
        std::roundf(1000000.0f / Game::FrameTimeStep.asMicroseconds())
        );
	window.setFramerateLimit(targetFrameRate);

    if (!GameAssets::Get().LoadAssets()) {
        fprintf(stderr, "ERROR - Failed to load game assets! Exiting\n");
        return EXIT_FAILURE;
    }

    if (!Game::Get().Init()) {
        fprintf(stderr, "ERROR - Failed to init game! Exiting\n");
        return EXIT_FAILURE;
    }

	while (window.isOpen()) {
		// handle window events
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;

            case sf::Event::KeyPressed:
#ifndef NDEBUG
                // F1 debug mode toggle
                if (event.key.code == sf::Keyboard::F1) {
                    Game::Get().SetDebugMode(!Game::Get().IsInDebugMode());
                }
#endif

                Game::Get().AddPressedEventKey(event.key.code);
                break;
			}
		}

		// game loop
        Game::Get().RunFrame(window);
		window.display();
	}

    printf("Exiting game\n");
    return EXIT_SUCCESS;
}
