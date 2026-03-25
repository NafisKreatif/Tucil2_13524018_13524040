#ifndef STIMA_PLANE_3D_H
#define STIMA_PLANE_3D_H
#include "Point3D.hpp"
#include "Line3D.hpp"

namespace stima {
    class Plane3D {
    public:
        Plane3D(Point3D p1, Point3D p2, Point3D p3);
        Point3D getNormal();
        bool isOnThePlane(Point3D& p);
        bool isParallel(Line3D& line);
        bool isParallel(Plane3D& plane);
        bool isIntersect(Line3D& line);
        Point3D getIntersection(Line3D& line);
        std::string toString();

    private:
        // Plane ax + by + cz + d = 0
        double a, b, c, d;
    };
}

#endif