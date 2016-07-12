#include <cstdlib>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Game.h"


int main(int argc, char* argv[])
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "UoLEduGame");
	window.setFramerateLimit(60);

	Game game(window);

	while (window.isOpen()) {
		// handle window events
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		// game loop
		game.RunFrame();
		window.display();
	}


    return EXIT_SUCCESS;
}
