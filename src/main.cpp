#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <iostream>
#include <array>
#include <X11/Xlib.h> 
#include "engine.hpp"

const auto aspect_ratio = 16.0 / 9.0;
const int image_width = 400;
const int image_height = static_cast<int>(image_width / aspect_ratio);

void renderImage(sf::RenderWindow& window) {
    sf::Texture texture;
    auto size = window.getSize();
    auto new_size = size;
    texture.create(size.x, size.y);
    Engine rtEngine(texture, size.x, size.y);
    sf::Sprite sprite(texture);

    while (window.isOpen())
    {
        window.clear();
        rtEngine.renderImage();
        window.draw(sprite);
        window.display();
    }
}

int main()
{
    XInitThreads();
    sf::VideoMode videomode(image_width, image_height);
    sf::RenderWindow window(videomode, "Ray Tracing Engine", sf::Style::Default & (~sf::Style::Resize));
    window.setActive(false);
    std::thread first(&renderImage, std::ref(window));

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
            }
        }
    }

    first.join();

    return 0;
}