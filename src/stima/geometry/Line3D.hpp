#ifndef STIMA_LINE_3D_H
#define STIMA_LINE_3D_H
#include "Point3D.hpp"

namespace stima {
    class Line3D {
    public:
        const Point3D p0, v;
        Line3D(Point3D p1, Point3D p2) : p0(p1), v(p2-p1) {};

    private:

    };
}

#endif