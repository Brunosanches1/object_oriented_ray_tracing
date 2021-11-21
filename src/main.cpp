#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <iostream>
#include <array>
#include <X11/Xlib.h> 

void create_image(sf::Uint8* pixels, int image_height, int image_width) {
    for(auto i = 0; i < image_height*image_width*4; i+=4) {

        // Get coordinates based on i
        // lin must begins from image_height -1 to 0 in order to print the image
        // in the correct rotation
        // This function is just to test
        int col = (i / 4) % image_width;
        int lin =(image_height-1) - (i / 4) / image_width;
        auto r = double(col) / (image_width-1);
        auto g = double(lin) / (image_height-1);
        auto b = 0.25;
        auto a = 1.0;

        sf::Uint8 ir = static_cast<sf::Uint8>(255.999 * r);
        sf::Uint8 ig = static_cast<sf::Uint8>(255.999 * g);
        sf::Uint8 ib = static_cast<sf::Uint8>(255.999 * b);
        sf::Uint8 ia = static_cast<sf::Uint8>(255.999 * a);

        pixels[i] = ir;
        pixels[i + 1] = ig;
        pixels[i + 2] = ib;
        pixels[i + 3] = ia;

    }
}

void renderImage(sf::RenderWindow& window) {
    sf::Texture texture;
    auto size = window.getSize();

    texture.create(size.x, size.y);
    sf::Sprite sprite(texture);
    
    std::cout << size.x << " " << size.y << std::endl;
    sf::Uint8* pixels = new sf::Uint8[size.x * size.y * 4];

    while (window.isOpen())
    {
        window.clear();
        create_image(pixels, size.x, size.y);
        texture.update(pixels);
        window.draw(sprite);
        window.display();
    }
}

int main()
{
    XInitThreads();
    sf::VideoMode videomode(256, 256);
    sf::RenderWindow window(videomode, "Ray Tracing Engine");
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
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    first.join();

    return 0;
}