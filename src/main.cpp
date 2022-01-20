#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <iostream>
#include <array>
#include <cstring>
#include <X11/Xlib.h> 
#include "engine.hpp"
#include "Gui/Menu.hpp"
#include "Gui/Theme.hpp"
#include "Gui/Gui.hpp"
#include "terminal_gui.hpp"

auto aspect_ratio = 3.0 / 2.0;
unsigned int image_width = 400;
unsigned int image_height = static_cast<unsigned int>(image_width / aspect_ratio); 

int main(int argc, char *argv[])
{ 
    char file_from[40], file_to[40], file_image_to[40];
    bool has_origin_file = false, has_dest_file=false, save_image=false;
    
    if (argc > 1) {
        for (auto i = 1; i < argc; i++) {
            if (strncmp(argv[i], "--from=", 7) == 0) {
                strcpy(file_from, argv[i]+7);
                has_origin_file = true;
                // std::cout << "Geting file from " << argv[i]+7 << std::endl;
            }
            else if (strncmp(argv[i], "--to=", 5) == 0) {
                strcpy(file_to, argv[i]+5);
                has_dest_file=true;
            }
            else if (strncmp(argv[i], "--save-image=", 13) == 0) {
                strcpy(file_image_to, argv[i]+13);
                save_image=true;
            }
        }
    } 
    
    XInitThreads();
    
    // window.setActive(false);

    //Engine rtEngine(texture, image_width, image_height);
    Engine rtEngine;
    if (has_origin_file) {
        rtEngine = Engine(file_from);
    } 
    else {
        rtEngine = Engine();
    }
    
    sf::Sprite sprite(rtEngine.getTexture());

    sf::VideoMode videomode(rtEngine.getImgWidth(), rtEngine.getImgHeight());
    sf::RenderWindow window(videomode, "Ray Tracing Engine", sf::Style::Default);

    termGui::init(window, rtEngine);
    //std::thread tGui(termGui::main_ncurses, std::ref(window), std::ref(rtEngine));

    // // Create the main window
    // sf::RenderWindow app(sf::VideoMode(640, 480), "SFML Widgets", sf::Style::Close);

    // gui::Menu menu(app);
    // menu.setPosition(10, 10);

    // sf::Text text("Hello world!", gui::Theme::getFont());
    // text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
    // text.setPosition(320, 360);

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
                // app.close();
            }

            // window.clear();
            // rtEngine.renderImage();
            // window.draw(sprite);
            // window.display();
        }
        window.clear();
        rtEngine.renderImage();
        window.draw(sprite);
        window.display();
    }

    if (has_dest_file) {
        // std::cout << "Saving file to " << file_to << std::endl;
        rtEngine.saveXmlDocument(file_to);
    }
    if (save_image) {
        // std::cout << "Saving image to " << file_image_to << std::endl;
        rtEngine.getTexture().copyToImage().saveToFile(file_image_to);
    }

    termGui::close();

    return 0;
}
