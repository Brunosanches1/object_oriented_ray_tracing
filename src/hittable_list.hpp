#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"
#include "aabb.hpp"

#include <memory>
#include <vector>

#include "../include/tinyxml2.h"

#include "material.hpp"

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
    public:
        hittable_list() {}  
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

		virtual bool bounding_box(
            double time0, double time1, aabb& output_box) const override;

        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const override;

        void saveXmlDocument(char* name);

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

void hittable_list::saveXmlDocument(char* name) {
    tinyxml2::XMLDocument xmlDoc;

    tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("Root");

    xmlDoc.InsertFirstChild(pRoot);

    pRoot->InsertEndChild(to_xml(xmlDoc));

    tinyxml2::XMLError eResult = xmlDoc.SaveFile(name);
}

#endif
