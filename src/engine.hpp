#include <SFML/Graphics.hpp>
#include <ostream>
#include "vector_stream.hpp"

#ifndef ENGINE_HPP
#define ENGINE_HPP

class Engine {
    private:
        sf::Texture& texture;

        int img_width;
        int img_height;  
        //VectorStream<sf::Uint8> pixels;
        std::vector<sf::Uint8> pixels;
        int changed = true;

    public:
        // Obligate programmer to pass parameters in order to create the engine
        Engine() = delete;

        Engine(const Engine&);

        Engine(sf::Texture&, int image_width, int img_height);

        void createImage();

        void renderImage();

        // Take arguments and change the object fields
        void renderImage(sf::Texture&);

        void renderImage(int new_img_width, int new_img_height);

        void renderImage(sf::Texture&, int new_img_width, int new_img_height);

};

#endif