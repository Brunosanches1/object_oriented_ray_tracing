#ifndef COLOR_H
#define COLOR_H

#include "vec3.hpp"
#include "rt.hpp"

#include <iostream>
#include "vector_stream.hpp"
#include <SFML/Graphics.hpp>


void write_color(VectorStream<sf::Uint8> &out, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<sf::Uint8>(255.999 * pixel_color.x())
        << static_cast<sf::Uint8>(255.999 * pixel_color.y())
        << static_cast<sf::Uint8>(255.999 * pixel_color.z());
}

// version avec 3 arguments :
void write_color(VectorStream<sf::Uint8> &out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    //~ // Write the translated [0,255] value of each color component.
    out << static_cast<sf::Uint8>(256 * clamp(r, 0.0, 0.999))
        << static_cast<sf::Uint8>(256 * clamp(g, 0.0, 0.999))
        << static_cast<sf::Uint8>(256 * clamp(b, 0.0, 0.999))
        << static_cast<sf::Uint8>(255.9999);

    std::cout << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif
