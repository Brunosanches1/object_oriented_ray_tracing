#include <SFML/Graphics.hpp>
#include <ostream>
#include <iostream>
#include <omp.h>
#include <vector>

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
        int samples_per_pixel;
        double aspect_ratio;
        int max_depth;

    public:
        // Obligate programmer to pass parameters in order to create the engine
        Engine() = delete;

        Engine(const Engine&);

        Engine(sf::Texture& texture, unsigned int image_width, unsigned int image_height, 
               int samples_per_pixel = 50,
               int max_depth = 20);

        Engine(sf::Texture& texture, unsigned int image_width, double aspect_ratio,
               int samples_per_pixel = 50,
               int max_depth = 20);

        void createImage();

        void renderImage();

        // Take arguments and change the object fields
        void renderImage(sf::Texture&);

        void renderImage(int new_img_width, int new_img_height);

        void renderImage(sf::Texture&, int new_img_width, int new_img_height);

};

#endif