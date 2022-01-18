#ifndef CAMERA_H
#define CAMERA_H

#include "rt.hpp"
#include "../include/tinyxml2.h"

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
            double _time1 = 0) {
				
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

            pElement->SetAttribute("LensRadius", lens_radius);
            pElement->SetAttribute("Time0", time0);
            pElement->SetAttribute("Time1", time1);
            
            tinyxml2::XMLElement* origin_xml = xmlDoc.NewElement("Origin");
            origin.to_xml(origin_xml);
            pElement->InsertEndChild(origin_xml);

            tinyxml2::XMLElement* lower_left_corner_xml = xmlDoc.NewElement("LowerLeftCorner");
            lower_left_corner.to_xml(lower_left_corner_xml);
            pElement->InsertEndChild(lower_left_corner_xml);

            tinyxml2::XMLElement* horizontal_xml = xmlDoc.NewElement("Horizontal");
            horizontal.to_xml(horizontal_xml);
            pElement->InsertEndChild(horizontal_xml);

            tinyxml2::XMLElement* vertical_xml = xmlDoc.NewElement("Vertical");
            vertical.to_xml(vertical_xml);
            pElement->InsertEndChild(vertical_xml);

            tinyxml2::XMLElement* u_xml = xmlDoc.NewElement("U");
            u.to_xml(u_xml);
            pElement->InsertEndChild(u_xml);

            tinyxml2::XMLElement* v_xml = xmlDoc.NewElement("V");
            v.to_xml(v_xml);
            pElement->InsertEndChild(v_xml);

            tinyxml2::XMLElement* w_xml = xmlDoc.NewElement("W");
            w.to_xml(w_xml);
            pElement->InsertEndChild(w_xml);

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
};
#endif
