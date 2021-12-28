#ifndef COLOR_H
#define COLOR_H

#include "vec3.hpp"
#include "rt.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

#include <iostream>

void write_color(std::vector<sf::Uint8> &out, color pixel_color, int samples_per_pixel, int lin, int col,
    int img_width) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    //~ // Write the translated [0,255] value of each color component.
    out[(lin * img_width + col) * 4] = static_cast<sf::Uint8>(256 * clamp(r, 0.0, 0.999));
    out[(lin * img_width + col) * 4 + 1] = static_cast<sf::Uint8>(256 * clamp(g, 0.0, 0.999));
    out[(lin * img_width + col) * 4 + 2] = static_cast<sf::Uint8>(256 * clamp(b, 0.0, 0.999));
    out[(lin * img_width + col) * 4 + 3] = static_cast<sf::Uint8>(255.9999);
}

#endif
