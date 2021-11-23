#include "engine.hpp"
#include <iostream>

Engine::Engine(sf::Texture& texture, int img_width, int img_height) :
    texture(texture), img_width(img_width), img_height(img_height) {
        
        pixels = new sf::Uint8[img_width*img_height*4];
    }

Engine::~Engine() {
    delete[] pixels;
}

void Engine::createImage() {
    for(auto i = 0; i < img_height*img_width*4; i+=4) {

        // Get coordinates based on i
        // lin must begins from img_height -1 to 0 in order to print the image
        // in the correct rotation
        // This function is just to test
        int col = (i/4) % (img_width);
        int lin =(img_height-1) - ((i / 4) / img_width);
        auto r = double(col) / (img_width-1);
        auto g = double(lin) / (img_height-1);
        auto b = 0.25;
        auto a = 1.0;

        // if(i == img_height*img_width*4 - 4) {
        //     std::cout << img_height << " " << img_width << std::endl;
        //     std::cout << ((i / 4) / img_width) << " " << col << std::endl;
        //     std::cout << r << " " << g << std::endl;
        // }
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

void Engine::renderImage() {
    createImage();
    texture.create(img_width, img_height);
    texture.update(pixels);
    //std::cout << texture.getSize().x << " " << texture.getSize().y << std::endl;
}

void Engine::renderImage(sf::Texture& new_texture) {
    texture = new_texture;
    renderImage();
}

void Engine::renderImage(int new_img_width, int new_img_height) {
    if (img_height != new_img_height || img_width != new_img_width) {
        img_height = new_img_height;
        img_width = new_img_width;

        delete[] pixels;
        pixels = new sf::Uint8[img_height*img_width*4];
    }
    
    renderImage();
}

void Engine::renderImage(sf::Texture& new_texture, int new_img_width, int new_img_height) {
    if (img_height != new_img_height || img_width != new_img_width) {
        img_height = new_img_height;
        img_width = new_img_width;

        delete[] pixels;
        pixels = new sf::Uint8[img_height*img_width*4];
    }
    texture = new_texture;
    
    renderImage();
}