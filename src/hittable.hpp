#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.hpp"
#include "rt.hpp"
#include "aabb.hpp"

#include "../include/tinyxml2.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr; //pointeur spécifique (cf tuto)
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class hittable {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
		virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const = 0;
};

#endif
