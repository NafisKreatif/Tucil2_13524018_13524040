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

const double PI = acos(-1);

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
struct Face{
    int v1, v2, v3;
    Face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
};

// Triangle class for painter's algorithm
struct Triangle {
    sf::Vector2f v1, v2, v3;
    float avgDepth;
    sf::Color color;
    
    bool operator<(const Triangle& other) const {
        return avgDepth-other.avgDepth < 1e-8;
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

struct Model {
    std::vector<Point3D> vertices;
    std::vector<Face> faces;
    Point3D center;
    float scale;
    
    bool loadOBJ(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            
            if (type == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                vertices.push_back(Point3D(x, y, z));
            }
            else if (type == "f") {
                std::string v1, v2, v3;
                iss >> v1 >> v2 >> v3;
                
                // Parse vertex indices (handle "v/vt/vn" format)
                int idx1 = std::stoi(v1) - 1;
                int idx2 = std::stoi(v2) - 1;
                int idx3 = std::stoi(v3) - 1;
                
                faces.push_back(Face(idx1, idx2, idx3));
            }
        }
        
        file.close();
        
        // Calculate center and normalize
        if (!vertices.empty()) {
            Point3D min = vertices[0], max = vertices[0];
            for (const auto& v : vertices) {
                min.x = std::min(min.x, v.x);
                min.y = std::min(min.y, v.y);
                min.z = std::min(min.z, v.z);
                max.x = std::max(max.x, v.x);
                max.y = std::max(max.y, v.y);
                max.z = std::max(max.z, v.z);
            }
            
            center = Point3D((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
            
            float maxDim = std::max({max.x - min.x, max.y - min.y, max.z - min.z});
            scale = 2.0f / maxDim;
            
            // Center and normalize vertices
            for (auto& v : vertices) {
                v = (v - center) * scale;
            }
        }
        
        return true;
    }
};

class Renderer{
    private:
        sf::RenderWindow& window;
        Camera camera;
        Model model;

        // Mouse info
        bool isDragging;
        sf::Vector2i lastMousePos;

        // Object rotation
        float objectRotationX;
        float objectRotationY;
        float objectRotationZ;

        // Light direction
        Point3D lightDir;

        // Base colour
        sf::Color baseColour;
        sf::Color bgColour;

    public:
        Renderer(sf::RenderWindow& window)
            : window(window), baseColour(128,128,128), bgColour(0,0,0), 
            objectRotationX(0), objectRotationY(0), objectRotationZ(0),
            isDragging(0),
            lightDir(Point3D(0.5f, -1.0f, 0.5f).normalize()) {}

        bool loadModel(const std::string& filename){
            bool success = model.loadOBJ(filename);
            if( success ){
                std::cout << "Loaded : " << filename << std::endl;
                std::cout << "Vertices: " << model.vertices.size() << std::endl;
                std::cout << "Faces: " << model.faces.size() << std::endl;
            }
            else{
                std::cout << "Fail to load : " << filename << std::endl;
            }
            return success;
        }

        sf::Color calculateLightning(const Point3D& normal) const {
            float intensity = std::abs(normal.x*lightDir.x + normal.y*lightDir.y + normal.z*lightDir.z);

            // Might need adjustments
            float ambient = 0.4f;
            intensity = ambient + (1.0 - ambient) * intensity;

            intensity = std::max(0.0f, std::min(1.0f, intensity));

            return sf::Color(
                static_cast<sf::Uint8>(baseColour.r * intensity),
                static_cast<sf::Uint8>(baseColour.g * intensity),
                static_cast<sf::Uint8>(baseColour.b * intensity)
            );
        }
       
        void render() {
            window.clear(bgColour);

            float width = static_cast<float>(window.getSize().x);
            float height = static_cast<float>(window.getSize().y);
            float aspectRatio = width / height;

            // Transformation matrix
            Mat4 rotX = Mat4::rotationX(objectRotationX);
            Mat4 rotY = Mat4::rotationY(objectRotationY);
            Mat4 rotZ = Mat4::rotationZ(objectRotationZ);
            Mat4 modelMat = rotZ * rotY * rotX;

            Mat4 view = camera.getViewMatrix();
            Mat4 projection = camera.getProjectionMatrix(aspectRatio, PI/4.0f, 0.1f, 100.0f);

            // Zoom matrices
            Mat4 zoomMat;
            zoomMat.m[0][0] = camera.zoom;
            zoomMat.m[1][1] = camera.zoom;
            zoomMat.m[2][2] = camera.zoom;
            
            Mat4 mat = projection * view * zoomMat * modelMat;

            // Transform vertices to screen
            std::vector<sf::Vector2f> screenVect;
            std::vector<float> depths;
            std::vector<Point3D> transformedVerts;
            
            for (const auto& v : model.vertices) {
                Point3D transformed = mat.transform(v);
                Point3D worldPos = modelMat.transform(v);
                
                // Convert to screen coordinates
                float screenX = (transformed.x + 1.0f) * width / 2.0f;
                float screenY = (1.0f - transformed.y) * height / 2.0f;
                
                screenVect.push_back(sf::Vector2f(screenX, screenY));
                depths.push_back(transformed.z);
                transformedVerts.push_back(worldPos);
            }

            // Triangle processing
            std::vector<Triangle> triangles;
            for (const auto& face : model.faces) {
                // Calculate face normal
                Point3D v1 = transformedVerts[face.v1];
                Point3D v2 = transformedVerts[face.v2];
                Point3D v3 = transformedVerts[face.v3];
                
                Point3D edge1 = v2 - v1;
                
                Point3D edge2 = v3 - v1;
                Point3D normal = edge1.crossProduct(edge2).normalize();

                // TODO : add checking
                bool shouldRender = true;
                
                if (shouldRender) {
                    Triangle tri;
                    tri.v1 = screenVect[face.v1];
                    tri.v2 = screenVect[face.v2];
                    tri.v3 = screenVect[face.v3];

                    // Calculate average depth
                    tri.avgDepth = (depths[face.v1] + depths[face.v2] + depths[face.v3]) / 3.0f;
                    tri.color = calculateLightning(normal);
                    
                    triangles.push_back(tri);
                }
            }

            // Sort by avgDepth non-increasing
            std::sort(triangles.begin(),triangles.end());

            // Fill colour
            for(const auto& tri : triangles) {
                // std::cout << tri.v1.x << " " << tri.v1.y << " " << tri.v2.x << " " << tri.v2.y << tri.v3.x << " " << tri.v3.y << std::endl; 
                sf::ConvexShape triangle(3);
                triangle.setPoint(0, tri.v1);
                triangle.setPoint(1, tri.v2);
                triangle.setPoint(2, tri.v3);
                triangle.setFillColor(tri.color);
                window.draw(triangle);
            }

            window.display();
        }

        void handleInput(sf::Event event) {
            if ( event.type == sf::Event::MouseButtonPressed ) {
                if ( event.mouseButton.button == sf::Mouse::Left ){
                    isDragging = true;
                    lastMousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                }
            }
            else if ( event.type == sf::Event::MouseButtonReleased ) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                }
            }
            else if ( event.type == sf::Event::MouseMoved ){
                if ( isDragging ){
                    sf::Vector2i curMousePos(event.mouseMove.x, event.mouseMove.y);
                    sf::Vector2i delta = curMousePos - lastMousePos;

                    objectRotationX += delta.y * 0.01f;
                    objectRotationY += delta.x * 0.01f;
                    lastMousePos = curMousePos; 
                }
            }
            else if ( event.type == sf::Event::MouseWheelScrolled ){
                int delta = event.mouseWheelScroll.delta;
                camera.zoom += delta * 0.1f;
                camera.zoom = std::max(0.1f, std::min(camera.zoom, 12.0f));
            }
        }
};

int main(int argc, char* argv[]) {
    // Get OBJ file from command line
    std::string objFile = argv[1];
    
    // Create window
    sf::RenderWindow window(sf::VideoMode(900, 900), "Tucil2_13524018_13524040");
    window.setFramerateLimit(60);
    
    Renderer renderer(window);
    
    // Load model
    if (!renderer.loadModel(objFile)) {
        std::cerr << "Failed to load .obj file: " << objFile << std::endl;
        return 1;
    }

    while (window.isOpen()) {
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect newArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(newArea));
            }
            renderer.handleInput(event);
        }

        renderer.render();
    }
    
    return 0;
}