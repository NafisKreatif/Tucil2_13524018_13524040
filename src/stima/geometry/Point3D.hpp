#ifndef STIMA_POINT_3D_H
#define STIMA_POINT_3D_H

namespace stima {
    class Point3D {
    public:
        // Attributes
        double x, y, z;

        // Constructor
        Point3D(double x, double y, double z);

        // Operators
        friend Point3D operator+(const Point3D &lhs, const Point3D &rhs);
        Point3D operator+=(const Point3D &rhs);
        friend Point3D operator-(const Point3D &lhs, const Point3D &rhs);
        Point3D operator-=(const Point3D &rhs);
        friend Point3D operator*(double lhs, const Point3D &rhs);
        friend Point3D operator*(const Point3D &lhs, double rhs);
        Point3D operator*=(double rhs);
        friend Point3D operator/(const Point3D &lhs, double rhs);
        Point3D operator/=(double rhs);

        // Methods
        double magnitude();
        Point3D normalize();
        double dotProduct(const Point3D &p);
        Point3D crossProduct(const Point3D &p);
    };
}

#endif