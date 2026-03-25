#ifndef STIMA_POINT_3D_H
#define STIMA_POINT_3D_H
#include <string>

namespace stima {
    class Point3D {
    public:
        // Attributes
        double x, y, z;

        // Constructor
        Point3D();
        Point3D(double x, double y, double z);

        // Mathematical Operators
        friend Point3D operator+(const Point3D &lhs, const Point3D &rhs);
        Point3D operator+=(const Point3D &rhs);
        friend Point3D operator-(const Point3D &lhs, const Point3D &rhs);
        Point3D operator-=(const Point3D &rhs);
        friend Point3D operator*(const Point3D &lhs, const Point3D &rhs);
        Point3D operator*=(const Point3D &rhs);
        friend Point3D operator*(double lhs, const Point3D &rhs);
        friend Point3D operator*(const Point3D &lhs, double rhs);
        Point3D operator*=(double rhs);
        friend Point3D operator/(const Point3D &lhs, double rhs);
        Point3D operator/=(double rhs);
        friend Point3D operator-(const Point3D &rhs);

        // Comparison Operators (for std set)
        bool operator<(const Point3D &rhs);
        bool operator<=(const Point3D &rhs);
        bool operator>(const Point3D &rhs);
        bool operator>=(const Point3D &rhs);

        // Methods
        double magnitude();
        Point3D normalize();
        double dotProduct(const Point3D &p);
        Point3D crossProduct(const Point3D &p);
        bool isApproximately(const Point3D &p, double epsilon = 1e-9);

        std::string toString();
    };
}

#endif