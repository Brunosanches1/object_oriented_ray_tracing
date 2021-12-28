#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "rt.hpp"
#include "aabb.hpp"
#include "hittable.hpp"

#include "../include/tinyxml2.h"

#include "material.hpp"

class moving_sphere : public hittable {
    public:
        moving_sphere() {}
        moving_sphere(
            point3 cen0, point3 cen1, double _time0, double _time1, double r, shared_ptr<material> m)
            : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m)
        {};
        moving_sphere(tinyxml2::XMLElement* pElement);

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

		virtual bool bounding_box(
            double _time0, double _time1, aabb& output_box) const override;
            
        point3 center(double time) const;

        virtual tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const override;

    public:
        point3 center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<material> mat_ptr;
};

moving_sphere::moving_sphere(tinyxml2::XMLElement* pElement) {
    radius = pElement->DoubleAttribute("Radius");
    time0 = pElement->DoubleAttribute("Time0");
    time1 = pElement->DoubleAttribute("Time1");

    tinyxml2::XMLElement* center0_xml = pElement->FirstChildElement("Center0");
    center0 = point3(center0_xml->DoubleAttribute("x"), center0_xml->DoubleAttribute("y"), center0_xml->DoubleAttribute("z"));

    tinyxml2::XMLElement* center1_xml = pElement->FirstChildElement("Center1");
    center1 = point3(center1_xml->DoubleAttribute("x"), center1_xml->DoubleAttribute("y"), center1_xml->DoubleAttribute("z"));

    mat_ptr = material::material_from_xml(pElement->FirstChildElement("Material"));
}

point3 moving_sphere::center(double time) const {
    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
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
    auto outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool moving_sphere::bounding_box(double _time0, double _time1, aabb& output_box) const {
    aabb box0(
        center(_time0) - vec3(radius, radius, radius),
        center(_time0) + vec3(radius, radius, radius));
    aabb box1(
        center(_time1) - vec3(radius, radius, radius),
        center(_time1) + vec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}

tinyxml2::XMLElement* moving_sphere::to_xml(tinyxml2::XMLDocument& xmlDoc) const {
    tinyxml2::XMLElement * pElement = xmlDoc.NewElement("Moving_Sphere");
    
    pElement->SetAttribute("Radius", radius);
    pElement->SetAttribute("Time0", time0);
    pElement->SetAttribute("Time1", time1);

    tinyxml2::XMLElement* center0_xml = xmlDoc.NewElement("Center0");

    center0_xml->SetAttribute("x", center0.x());
    center0_xml->SetAttribute("y", center0.y());
    center0_xml->SetAttribute("z", center0.z());

    pElement->InsertEndChild(center0_xml);
    
    tinyxml2::XMLElement* center1_xml = xmlDoc.NewElement("Center1");

    center1_xml->SetAttribute("x", center1.x());
    center1_xml->SetAttribute("y", center1.y());
    center1_xml->SetAttribute("z", center1.z());

    pElement->InsertEndChild(center1_xml);

    tinyxml2::XMLElement* material_xml = xmlDoc.NewElement("Material");
    tinyxml2::XMLElement* materialElement = mat_ptr->to_xml(xmlDoc);
    
    material_xml->InsertEndChild(materialElement);

    pElement->InsertEndChild(material_xml);
    
    return pElement;
}

#endif
