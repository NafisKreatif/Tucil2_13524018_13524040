#include "SFML/Graphics.hpp"
#include "stima/geometry/Point3D.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>
#include <cmath>
using namespace stima;

// Mat 4x4 for transformations
struct Mat4 {
    float m[4][4];

    Mat4() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Rotation by axis
    static Mat4 rotationX(float angle) {
        Mat4 mat;
        float c = std::cos(angle);
        float s = std::sin(angle);
        mat.m[1][1] = c; mat.m[1][2] = -s;
        mat.m[2][1] = s; mat.m[2][2] = c;
        return mat;
    }
    
    static Mat4 rotationY(float angle) {
        Mat4 mat;
        float c = std::cos(angle);
        float s = std::sin(angle);
        mat.m[0][0] = c; mat.m[0][2] = s;
        mat.m[2][0] = -s; mat.m[2][2] = c;
        return mat;
    }
    
    static Mat4 rotationZ(float angle) {
        Mat4 mat;
        float c = std::cos(angle);
        float s = std::sin(angle);
        mat.m[0][0] = c; mat.m[0][1] = -s;
        mat.m[1][0] = s; mat.m[1][1] = c;
        return mat;
    }

    // Operators
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Point transformation
    Point3D transform(const Point3D& v) const {
        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];
        return Point3D(
            (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) / w,
            (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) / w,
            (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) / w
        );
    }

    // Vector (direction/normal) transformation [No translation]
    Point3D transformNormal(const Point3D& v) const {
        return Point3D(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        ).normalize();
    }
};

// Triangle face
class Face{
    int v1, v2, v3;
    Face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
};

// Triangle class for painter's algorithm
struct Triangle {
    sf::Vector2f v1, v2, v3;
    float avgDepth;
    sf::Color color;
    
    bool operator<(const Triangle& other) const {
        return avgDepth > other.avgDepth;
    }
};

// Camera class
struct Camera {
    Point3D position;
    float zoom;

    // Default camera position
    Camera() : position(0, 0, -5), zoom(1.0f) {}

    // Returns the translation matrix from camera position
    Mat4 getViewMatrix() const {
        Mat4 translation;
        translation.m[0][3] = -position.x;
        translation.m[1][3] = -position.y;
        translation.m[2][3] = -position.z;
        
        return translation;
    }

    // Projection matrix ( Range : [-1,1] )
    Mat4 getProjectionMatrix(float aspectRatio, float fov, float near, float far) const {
        Mat4 proj;
        float tanFov = std::tan(fov / 2.0f);
        
        proj.m[0][0] = 1.0f / (aspectRatio * tanFov);
        proj.m[1][1] = 1.0f / tanFov;
        proj.m[2][2] = -(far + near) / (far - near);
        proj.m[2][3] = -(2.0f * far * near) / (far - near);
        proj.m[3][2] = -1.0f;
        proj.m[3][3] = 0.0f;
        
        return proj;
    }
};

class Renderer{

};