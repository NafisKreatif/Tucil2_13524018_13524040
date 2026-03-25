#include "Plane3D.hpp"
#include "Line3D.hpp"
#include "Point3D.hpp"
#include <cmath>
#include <stdexcept>

namespace stima {
    Plane3D::Plane3D(Point3D p1, Point3D p2, Point3D p3)
    {
        Point3D v1 = p2 - p1;
        Point3D v2 = p3 - p1;
        Point3D normal = v1.crossProduct(v2).normalize();
        a = normal.x;
        b = normal.y;
        c = normal.z;
        d = -(a * p1.x + b * p1.y + c * p1.z);
    }

    Point3D Plane3D::getNormal()
    {
        return Point3D(a, b, c);
    }

    bool Plane3D::isOnThePlane(Point3D& p)
    {
        return std::abs(a * p.x + b * p.y + c * p.z + d) < 1e-9;
    }
    bool Plane3D::isParallel(Line3D& line)
    {
        return std::abs(getNormal().dotProduct(line.v)) < 1e-9;
    }
    bool Plane3D::isParallel(Plane3D& plane)
    {
        return getNormal().isApproximately(plane.getNormal());
    }

    bool Plane3D::isIntersect(Line3D& line)
    {
        return std::abs(getNormal().dotProduct(line.v)) > 1e-9;
    }

    Point3D Plane3D::getIntersection(Line3D& line)
    {
        if (!isIntersect(line)) {
            throw std::invalid_argument("The plane does not have intersection with the line");
        }
        // Penyelesaian persamaan
        // Garis = p0 - tv -> p0 = titik; v = vektor arah
        // x = p0.x - t * v.x
        // y = p0.y - t * v.y
        // z = p0.z - t * v.z
        // Subtitusi ke ax + by + cz + d = 0, lalu cari t
        // Masukkan t ke persamaan garis, dapat intersectionnya :D
        double t = (a * line.p0.x + b * line.p0.y + c * line.p0.z + d) / (a * line.v.x + b * line.v.y + c * line.v.z);
        return line.p0 - t * line.v;
    };

    std::string Plane3D::toString()
    {
        return std::to_string(a) + "x + " + std::to_string(b) + "y + " + std::to_string(c) + "z + " + std::to_string(d) + " = 0";
    }
}