#include <SFML/Graphics.hpp>

#ifndef ENGINE_HPP
#define ENGINE_HPP

class Engine {
    private:
        sf::Uint8* pixels;
        sf::Texture& texture;

        int img_width;
        int img_height;        

    public:
        // Obligate programmer to pass parameters in order to create the engine
        Engine() = delete;

        Engine(sf::Texture&, int image_width, int img_height);

        ~Engine();

        void createImage();

        void renderImage();

        // Take arguments and change the object fields
        void renderImage(sf::Texture&);

        void renderImage(int new_img_width, int new_img_height);

        void renderImage(sf::Texture&, int new_img_width, int new_img_height);
};

#endif