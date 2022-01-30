#ifndef CAMERA_H
#define CAMERA_H

#include "rt.hpp"
#include "../include/tinyxml2.h"
#include <iostream>

class camera {
    public:
        camera() {};

        camera(
			point3 lookfrom,
            point3 lookat,
            vec3   vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist,double _time0 = 0,
            double _time1 = 0) : lookat(lookat), vup(vup), vfov(vfov), aspect_ratio(aspect_ratio), aperture(aperture), focus_dist(focus_dist) {
				
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            origin = lookfrom;
            
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
            time0 = _time0;
            time1 = _time1;
            
            
            //~ auto focal_length = 1.0;

            //~ origin = point3(0, 0, 0);
            //~ horizontal = vec3(viewport_width, 0.0, 0.0);
            //~ vertical = vec3(0.0, viewport_height, 0.0);
            //~ lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
        }

        camera(tinyxml2::XMLElement * pElement) {
            aperture = pElement->DoubleAttribute("Aperture");
            vfov = pElement->DoubleAttribute("Vfov");
            aspect_ratio = pElement->DoubleAttribute("AspectRatio");
            focus_dist = pElement->DoubleAttribute("FocusDist");
            time0 = pElement->DoubleAttribute("Time0");
            time1 = pElement->DoubleAttribute("Time1");

            tinyxml2::XMLElement * pLookFromElement = pElement->FirstChildElement("LookFrom");
            if (pLookFromElement == nullptr) throw std::invalid_argument("Camera Element does not have a LookFrom element");

            origin = vec3(pLookFromElement);

            tinyxml2::XMLElement * pLookAtElement = pElement->FirstChildElement("LookAt");
            if (pLookAtElement == nullptr) throw std::invalid_argument("Camera Element does not have a LookAt element");

            lookat = vec3(pLookAtElement);

            tinyxml2::XMLElement * pVupElement = pElement->FirstChildElement("Vup");
            if (pVupElement == nullptr) throw std::invalid_argument("Camera Element does not have a Vup element");

            vup = vec3(pVupElement);

            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            w = unit_vector(origin - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;

        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(
                origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset,
                random_double(time0, time1)
            );
        }

        tinyxml2::XMLElement* to_xml(tinyxml2::XMLDocument& xmlDoc) const {
            tinyxml2::XMLElement * pElement = xmlDoc.NewElement("Camera");

            // pElement->SetAttribute("LensRadius", lens_radius);
            pElement->SetAttribute("Aperture", aperture);
            pElement->SetAttribute("Vfov", vfov);
            pElement->SetAttribute("AspectRatio", aspect_ratio);
            pElement->SetAttribute("FocusDist", focus_dist);
            pElement->SetAttribute("Time0", time0);
            pElement->SetAttribute("Time1", time1);
            
            tinyxml2::XMLElement* look_from_xml = xmlDoc.NewElement("LookFrom");
            origin.to_xml(look_from_xml);
            pElement->InsertEndChild(look_from_xml);

            tinyxml2::XMLElement* lookat_xml = xmlDoc.NewElement("LookAt");
            lookat.to_xml(lookat_xml);
            pElement->InsertEndChild(lookat_xml);

            // tinyxml2::XMLElement* lower_left_corner_xml = xmlDoc.NewElement("LowerLeftCorner");
            // lower_left_corner.to_xml(lower_left_corner_xml);
            // pElement->InsertEndChild(lower_left_corner_xml);

            // tinyxml2::XMLElement* horizontal_xml = xmlDoc.NewElement("Horizontal");
            // horizontal.to_xml(horizontal_xml);
            // pElement->InsertEndChild(horizontal_xml);

            // tinyxml2::XMLElement* vertical_xml = xmlDoc.NewElement("Vertical");
            // vertical.to_xml(vertical_xml);
            // pElement->InsertEndChild(vertical_xml);

            // tinyxml2::XMLElement* u_xml = xmlDoc.NewElement("U");
            // u.to_xml(u_xml);
            // pElement->InsertEndChild(u_xml);

            tinyxml2::XMLElement* vup_xml = xmlDoc.NewElement("Vup");
            vup.to_xml(vup_xml);
            pElement->InsertEndChild(vup_xml);

            // tinyxml2::XMLElement* w_xml = xmlDoc.NewElement("W");
            // w.to_xml(w_xml);
            // pElement->InsertEndChild(w_xml);

            return pElement;

        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
        double time0, time1;  // shutter open/close times

        // Variables to save parameters
        point3 lookat;
        vec3 vup;
        double vfov, aspect_ratio, aperture, focus_dist;

};
#endif
