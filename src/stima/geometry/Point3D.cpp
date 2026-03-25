#include "Point3D.hpp"
#include <math.h>
#include <string>

namespace stima {
    // Constructor
    Point3D::Point3D() : Point3D(0, 0, 0) {};
    Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

    // Operators
    Point3D operator+(const Point3D &lhs, const Point3D &rhs)
    {
        return Point3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }
    Point3D Point3D::operator+=(const Point3D &rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }
    Point3D operator-(const Point3D &lhs, const Point3D &rhs)
    {
        return Point3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }
    Point3D Point3D::operator-=(const Point3D &rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }
    Point3D operator*(const Point3D &lhs, const Point3D &rhs)
    {
        return Point3D(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }
    Point3D Point3D::operator*=(const Point3D &rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        this->z *= rhs.z;
        return *this;
    }
    Point3D operator*(double lhs, const Point3D &rhs)
    {
        return Point3D(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
    }
    Point3D operator*(const Point3D &lhs, double rhs)
    {
        return Point3D(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    }
    Point3D Point3D::operator*=(double rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        return *this;
    }
    Point3D operator/(const Point3D &lhs, double rhs)
    {
        return Point3D(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
    }
    Point3D operator-(const Point3D &rhs)
    {
        return rhs * -1;
    }
    Point3D Point3D::operator/=(double rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
        return *this;
    }
    bool Point3D::operator<(const Point3D &rhs)
    {
        if (this->x < rhs.x) return true;
        if (this->x > rhs.x) return false;
        else {
            if (this->y < rhs.y) return true;
            if (this->y > rhs.y) return false;
            else {
                return this->z < rhs.z;
            }
        }
    }
    bool Point3D::operator<=(const Point3D &rhs)
    {
        if (this->x < rhs.x) return true;
        if (this->x > rhs.x) return false;
        else {
            if (this->y < rhs.y) return true;
            if (this->y > rhs.y) return false;
            else {
                return this->z <= rhs.z;
            }
        }
    }
    bool Point3D::operator>(const Point3D &rhs)
    {
        if (this->x < rhs.x) return false;
        if (this->x > rhs.x) return true;
        else {
            if (this->y < rhs.y) return false;
            if (this->y > rhs.y) return true;
            else {
                return this->z > rhs.z;
            }
        }
    }
    bool Point3D::operator>=(const Point3D &rhs)
    {
        if (this->x < rhs.x) return false;
        if (this->x > rhs.x) return true;
        else {
            if (this->y < rhs.y) return false;
            if (this->y > rhs.y) return true;
            else {
                return this->z >= rhs.z;
            }
        }
    }

    // Methods
    double Point3D::magnitude()
    {
        return sqrt(x * x + y * y + z * z);
    }
    Point3D Point3D::normalize()
    {
        return *this / magnitude();
    }
    double Point3D::dotProduct(const Point3D &p)
    {
        return x * p.x + y * p.y + z * p.z;
    }
    Point3D Point3D::crossProduct(const Point3D &p)
    {
        return Point3D(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }
    bool Point3D::isApproximately(const Point3D &p, double epsilon)
    {
        return std::abs(x - p.x) < epsilon && std::abs(y - p.y) < epsilon && std::abs(z - p.z) < epsilon;
    }

    std::string Point3D::toString()
    {
        return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
    }
}