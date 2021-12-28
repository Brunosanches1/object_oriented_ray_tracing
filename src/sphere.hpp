#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "vec3.hpp"

#include "../include/tinyxml2.h"

#include "material.hpp"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(point3 cen, double r) : center(cen), radius(r) {};
        sphere(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};
        sphere(tinyxml2::XMLElement* pElement);

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

		virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const override;
		
    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

sphere::sphere(tinyxml2::XMLElement* pElement) {
    radius = pElement->DoubleAttribute("Radius");

    tinyxml2::XMLElement* center_xml = pElement->FirstChildElement("Center");
    center = point3(center_xml->DoubleAttribute("x"), center_xml->DoubleAttribute("y"), center_xml->DoubleAttribute("z"));

    mat_ptr = material::material_from_xml(pElement->FirstChildElement("Material"));
}

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
}

tinyxml2::XMLElement* sphere::to_xml(tinyxml2::XMLDocument& xmlDoc) const {
    tinyxml2::XMLElement * pElement = xmlDoc.NewElement("Sphere");
    
    pElement->SetAttribute("Radius", radius);
    
    tinyxml2::XMLElement* center_xml = xmlDoc.NewElement("Center");

    center_xml->SetAttribute("x", center.x());
    center_xml->SetAttribute("y", center.y());
    center_xml->SetAttribute("z", center.z());

    pElement->InsertEndChild(center_xml);

    tinyxml2::XMLElement* material_xml = xmlDoc.NewElement("Material");
    tinyxml2::XMLElement* materialElement = mat_ptr->to_xml(xmlDoc);
    
    material_xml->InsertEndChild(materialElement);

    pElement->InsertEndChild(material_xml);
    
    return pElement;
}


#endif
