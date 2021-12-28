#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <iostream>
#include <array>
#include <X11/Xlib.h> 
#include "engine.hpp"
#include "Gui/Menu.hpp"
#include "Gui/Theme.hpp"
#include "Gui/Gui.hpp"

auto aspect_ratio = 3.0 / 2.0;
unsigned int image_width = 400;
unsigned int image_height = static_cast<unsigned int>(image_width / aspect_ratio); 


int main()
{ 

    XInitThreads();

    sf::VideoMode videomode(image_width, image_height);
    sf::RenderWindow window(videomode, "Ray Tracing Engine", sf::Style::Default);
    
    window.setActive(false);

    sf::Texture texture;

    texture.create(image_width, image_height);
    Engine rtEngine(texture, image_width, image_height);
    sf::Sprite sprite(texture);

    // Create the main window
    sf::RenderWindow app(sf::VideoMode(640, 480), "SFML Widgets", sf::Style::Close);

    gui::Menu menu(app);
    menu.setPosition(10, 10);

    sf::Text text("Hello world!", gui::Theme::getFont());
    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
    text.setPosition(320, 360);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                window.close();
                app.close();
            }

            window.clear();
            rtEngine.renderImage();
            window.draw(sprite);
            window.display();
        }
    }

    return 0;
}
