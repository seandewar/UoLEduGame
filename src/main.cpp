#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Game.h"


int main(int argc, char* argv[])
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "The File System Dungeon - Loading...");

    const auto targetFrameRate = static_cast<unsigned int>(
        std::roundf(1000000.0f / Game::FrameTimeStep.asMicroseconds())
        );
	window.setFramerateLimit(targetFrameRate);

    if (!GameAssets::Get().LoadAssets()) {
        std::cerr << "ERROR - Failed to load game assets! Exiting\n";
        return EXIT_FAILURE;
    }

    if (!Game::Get().Init()) {
        std::cerr << "ERROR - Failed to init game! Exiting\n";
        return EXIT_FAILURE;
    }

    window.setTitle("The File System Dungeon");

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

        // automatically pause if window not in focus
        if (!window.hasFocus() && !Game::Get().IsPaused()) {
            std::cout << "Window lost focus - pausing game\n";
            Game::Get().SetPaused(true);
        }

		// game loop
        Game::Get().RunFrame(window);
		window.display();
	}

    std::cout << "Exiting game\n";
    return EXIT_SUCCESS;
}


#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
