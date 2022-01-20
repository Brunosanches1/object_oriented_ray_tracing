#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"
#include "sphere.hpp"
#include "moving_sphere.hpp"
#include "aabb.hpp"

#include <memory>
#include <vector>
#include <iostream>
#include <cstring>

#include "../include/tinyxml2.h"

#include "material.hpp"

using std::shared_ptr;
using std::make_shared;

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

class hittable_list : public hittable {
    public:
        hittable_list() {}  
        hittable_list(shared_ptr<hittable> object) { add(object); }
        hittable_list(const char* xml_filename);
        hittable_list(tinyxml2::XMLElement * pElement);

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

		virtual bool bounding_box(
            double time0, double time1, aabb& output_box) const override;

        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const override;

        void saveXmlDocument(char* filename);

    public:
        std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool hittable_list::bounding_box(double time0, double time1, aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

tinyxml2::XMLElement* hittable_list::to_xml(tinyxml2::XMLDocument& xmlDoc) const {
    tinyxml2::XMLElement * pElement = xmlDoc.NewElement("List");

    for (auto & item : objects)
    {
        tinyxml2::XMLElement * pListElement = item->to_xml(xmlDoc);

        pElement->InsertEndChild(pListElement);
    }

    return pElement;
}

// void hittable_list::saveXmlDocument(char* filename) {
//     tinyxml2::XMLDocument xmlDoc;

//     tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("Root");

//     xmlDoc.InsertFirstChild(pRoot);

//     pRoot->InsertEndChild(to_xml(xmlDoc));

//     xmlDoc.SaveFile(filename);
// }

hittable_list::hittable_list(tinyxml2::XMLElement * pElement) {
    tinyxml2::XMLElement * pListElement = pElement->FirstChildElement();
    while (pListElement != nullptr)
    {
        if (strcmp(pListElement->Name(), "Sphere") == 0) {
            objects.push_back(make_shared<sphere>(pListElement));
        }
        else if (strcmp(pListElement->Name(), "Moving_Sphere") == 0) {
            objects.push_back(make_shared<moving_sphere>(pListElement));
        }
        else {
            throw std::invalid_argument("Object not defined or list inside list");
        }

        pListElement = pListElement->NextSiblingElement();
    }
}

// hittable_list::hittable_list(const char* xml_filename) {
//     tinyxml2::XMLDocument xmlDoc;

//     tinyxml2::XMLError eResult = xmlDoc.LoadFile(xml_filename);
//     XMLCheckResult(eResult);

//     tinyxml2::XMLNode * pRoot = xmlDoc.FirstChild();
//     if (pRoot == nullptr) throw std::invalid_argument("File does not contain a root element");

//     tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("List");
//     if (pElement == nullptr) throw std::invalid_argument("File does not contain a list element");

//     tinyxml2::XMLElement * pListElement = pElement->FirstChildElement();
//     while (pListElement != nullptr)
//     {
//         if (strcmp(pListElement->Name(), "Sphere") == 0) {
//             objects.push_back(make_shared<sphere>(pListElement));
//         }
//         else if (strcmp(pListElement->Name(), "Moving_Sphere") == 0) {
//             objects.push_back(make_shared<moving_sphere>(pListElement));
//         }
//         else {
//             throw std::invalid_argument("Object not defined or list inside list");
//         }

//         pListElement = pListElement->NextSiblingElement();
//     }

// }

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.33) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                    // world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.66) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

#endif
