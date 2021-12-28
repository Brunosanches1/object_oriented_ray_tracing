#include "engine.hpp"
#include "hittable_list.hpp"
#include "color.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "rt.hpp"
#include "camera.hpp"
#include "material.hpp"

Engine::Engine(sf::Texture& texture, unsigned int image_width, unsigned int image_height, 
               int samples_per_pixel,
               int max_depth) :
    texture(texture), img_width(image_width), img_height(image_height), pixels(img_width*img_height*4),
    samples_per_pixel(samples_per_pixel), aspect_ratio(img_width / img_height), max_depth(max_depth)  {}

Engine::Engine(sf::Texture& texture, unsigned int image_width, double aspect_ratio, 
               int samples_per_pixel,
               int max_depth) :
    texture(texture), img_width(image_width), img_height(img_width/aspect_ratio), pixels(img_width*img_height*4),
    samples_per_pixel(samples_per_pixel), aspect_ratio(aspect_ratio), max_depth(max_depth) {}

// Return color of a ray
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;
    
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);
        
    if (world.hit(r, 0.001, infinity, rec)) {        
        ray scattered;
        color attenuation;

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

void Engine::createImage() 
{
    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    int samples_per_pixel = 50;
    const int max_depth = 20; // param à modifier pour aller moins profondément pour la récursivité : 50 de base
    
    // World
	
	auto world = random_scene();

    // Camera

    //~ camera cam(point3(-2,2,1), point3(0,0,-1), vec3(0,1,0), 20, aspect_ratio);
	point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    
	vec3 vup(0,1,0);
	
	auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
		
	// Render
    if (changed) {
          // Render
	    [[gnu::unused]] // pour spécifier que s ne sera pas utilisé
	    int s; // pour que omp reconnaisse s en private
        pixels.clear();
        std::cout << "P3\n" << img_width << ' ' << img_height
         << "\n255\n";

        #pragma omp parallel for schedule(dynamic)
        for (int j = img_height-1; j >= 0; --j) {

            auto tid = omp_get_thread_num();
            if(tid == 0) {
                std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush; 
            }

            for (int i = 0; i < img_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (img_width-1);
                    auto v = (j + random_double()) / (img_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, max_depth);
                }
                write_color(pixels, pixel_color, samples_per_pixel, (img_height-1) - j, i, img_width);
            }
        }
        changed=false;
    }
	
}

void Engine::renderImage() {
    createImage();
    texture.create(img_width, img_height);
    texture.update(pixels.data());
}

void Engine::renderImage(sf::Texture& new_texture) {
    texture = new_texture;
    renderImage();
}

void Engine::renderImage(int new_img_width, int new_img_height) {
    if (img_height != new_img_height || img_width != new_img_width) {
        img_height = new_img_height;
        img_width = new_img_width;

        pixels.resize(img_height*img_width*4);
    }
    
    renderImage();
}

void Engine::renderImage(sf::Texture& new_texture, int new_img_width, int new_img_height) {
    if (img_height != new_img_height || img_width != new_img_width) {
        img_height = new_img_height;
        img_width = new_img_width;

        pixels.resize(img_height*img_width*4);
    }
    texture = new_texture;
    
    renderImage();
}
