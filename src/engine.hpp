#include <SFML/Graphics.hpp>
#include <ostream>
#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>


#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "hittable_list.hpp"
#include "color.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "rt.hpp"
#include "camera.hpp"
#include "material.hpp"

class Engine {
    private:
        sf::Texture texture;

        int img_width;
        int img_height;  
        //VectorStream<sf::Uint8> pixels;
        std::vector<sf::Uint8> pixels;
        int samples_per_pixel;
        double aspect_ratio;
        int max_depth;
        hittable_list world;
        camera cam;
        bool has_image=false;
        
        /* variables to enable progress bar */
        bool working = false;
        int remaining_lines = 0;
        std::chrono::time_point<std::chrono::steady_clock> start_time;

    public:
        Engine();

        Engine(const Engine&);

        Engine(unsigned int image_width, unsigned int image_height, 
               int samples_per_pixel = 50,
               int max_depth = 20);

        Engine(unsigned int image_width, double aspect_ratio = 1.2,
               int samples_per_pixel = 50,
               int max_depth = 20);

        Engine(const char* xml_filename);

        void saveXmlDocument(const char* filename) const;

        void createImage();

        void renderImage();

        // Take arguments and change the object fields
        void renderImage(sf::Texture&);

        void renderImage(int new_img_width, int new_img_height);

        void renderImage(sf::Texture&, int new_img_width, int new_img_height);

        void setSamplesPerPixel(int value) {
            samples_per_pixel = value;
        }

        void setMaxDepth(int value) {
            max_depth = value;
        }

        void setAspectRatio(double value) {
            aspect_ratio = value;
            img_height = static_cast<int>(img_width / aspect_ratio);
        }

        void setImageWidth(int value) {
            img_width = value;
            aspect_ratio = static_cast<double>(img_width / img_height);
        }

        void setImageHeight(int value) {
            img_height = value;
            aspect_ratio = static_cast<double>(img_width / img_height);
        }

        void setToWork() {
            working = true;
        }

        bool hasImageReady() { return has_image; }
        sf::Texture& getTexture() { return texture; }
        int getImgWidth() { return img_width; }
        int getImgHeight() { return img_height; }

        /* Progress bar useful methods */
        bool isWorking() { return working; }
        std::chrono::time_point<std::chrono::steady_clock> workStartTime() { return start_time; }
        int getRemainingLines() { return remaining_lines; }

};

Engine::Engine() : img_width(480), img_height(400), pixels(4*img_width*img_height),
    samples_per_pixel(100), max_depth(50) {
        texture = sf::Texture();
        texture.create(img_width, img_height);
        aspect_ratio = (double) img_width/img_height;
        point3 lookfrom(13,2,3);
        point3 lookat(0,0,0);
        
        vec3 vup(0,1,0);
        
        auto dist_to_focus = 10.0;
        auto aperture = 0.1;

        cam = camera(lookfrom, lookat, vup, 20.0, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
        world = random_scene();
    }

Engine::Engine(unsigned int image_width, unsigned int image_height, 
               int samples_per_pixel,
               int max_depth) :
    img_width(image_width), img_height(image_height), pixels(img_width*img_height*4),
    samples_per_pixel(samples_per_pixel), aspect_ratio(img_width / img_height), max_depth(max_depth)  {

        texture = sf::Texture();
        texture.create(img_width, img_height);
        point3 lookfrom(13,2,3);
        point3 lookat(0,0,0);
        
        vec3 vup(0,1,0);
        
        auto dist_to_focus = 10.0;
        auto aperture = 0.1;

        cam = camera(lookfrom, lookat, vup, 20.0, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
        world = random_scene();
    }

Engine::Engine(unsigned int image_width, double aspect_ratio, 
               int samples_per_pixel,
               int max_depth) :
    img_width(image_width), img_height(img_width/aspect_ratio), pixels(img_width*img_height*4),
    samples_per_pixel(samples_per_pixel), aspect_ratio(aspect_ratio), max_depth(max_depth) {
        texture = sf::Texture();
        texture.create(img_width, img_height);
        point3 lookfrom(13,2,3);
        point3 lookat(0,0,0);
        
        vec3 vup(0,1,0);
        
        auto dist_to_focus = 10.0;
        auto aperture = 0.1;

        cam = camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
        world = random_scene();
    }

Engine::Engine(const char* filename) {
    tinyxml2::XMLDocument xmlDoc;

    tinyxml2::XMLError eResult = xmlDoc.LoadFile(filename);
    //XMLCheckResult(eResult);

    tinyxml2::XMLNode * pRoot = xmlDoc.FirstChild();
    if (pRoot == nullptr) throw std::invalid_argument("File does not contain a root element");

    tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("Engine");
    if (pElement == nullptr) throw std::invalid_argument("File does not contain an engine element");

    img_width = pElement->IntAttribute("ImgWidth");
    img_height = pElement->IntAttribute("ImgHeight");
    samples_per_pixel = pElement->IntAttribute("SamplesPerPixel");
    aspect_ratio = pElement->DoubleAttribute("AspectRatio");
    max_depth = pElement->IntAttribute("MaxDepth");

    pixels = std::vector<sf::Uint8>(4*img_width*img_height);
    texture = sf::Texture();
    texture.create(img_width, img_height);

    tinyxml2::XMLElement * pCameraElement = pElement->FirstChildElement("Camera");
    if (pCameraElement == nullptr) throw std::invalid_argument("File does not contain a camera element");

    cam = camera(pCameraElement);

    tinyxml2::XMLElement * pListElement = pRoot->FirstChildElement("List");
    if (pListElement == nullptr) throw std::invalid_argument("File does not contain a list element");

    world = hittable_list(pListElement);

}

void Engine::saveXmlDocument(const char* filename) const{
    tinyxml2::XMLDocument xmlDoc;

    tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("Root");
    xmlDoc.InsertFirstChild(pRoot);

    tinyxml2::XMLElement * pElement = xmlDoc.NewElement("Engine");

    pElement->SetAttribute("ImgWidth", img_width);
    pElement->SetAttribute("ImgHeight", img_height);
    pElement->SetAttribute("SamplesPerPixel", samples_per_pixel);
    pElement->SetAttribute("AspectRatio", aspect_ratio);
    pElement->SetAttribute("MaxDepth", max_depth);

    pElement->InsertEndChild(cam.to_xml(xmlDoc));
    pRoot->InsertEndChild(pElement);

    pRoot->InsertEndChild(world.to_xml(xmlDoc));

    xmlDoc.SaveFile(filename);
}

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
    // Camera

    //~ camera cam(point3(-2,2,1), point3(0,0,-1), vec3(0,1,0), 20, aspect_ratio);
	// point3 lookfrom(13,2,3);
    // point3 lookat(0,0,0);
    
	// vec3 vup(0,1,0);
	
	// auto dist_to_focus = 10.0;
    // auto aperture = 0.1;

    // cam = camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
	
    //saveXmlDocument("data/RandomWorld.xml");

	// Render
    if (working) {
        // Render
        [[gnu::unused]] // pour spécifier que s ne sera pas utilisé
        int s; // pour que omp reconnaisse s en private
        pixels.clear();
        // std::cout << "P3\n" << img_width << ' ' << img_height
        //  << "\n255\n";

        // working = true;
        start_time = std::chrono::steady_clock::now();
        for (int j = img_height-1; j >= 0; --j) {
            // std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush; 
            remaining_lines = j;
            #pragma omp parallel for schedule(dynamic, 10)
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
        working = false;
        has_image = true;
        // work=false;
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

#endif