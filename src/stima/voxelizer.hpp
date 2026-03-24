#ifndef STIMA_VOXELIZER_H
#define STIMA_VOXELIZER_H
#include "stima/geometry.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace stima {
    class Voxelizer {
    public:
        void voxelize(std::string inputFilePath, int maxDepth);
        int getVoxelCount();
        const std::vector<int> &getTraversedCount();
        const std::vector<int> &getNotTraversedCount();
        const std::vector<Point3D> &getResultVertices();
        const std::vector<std::tuple<int, int, int>>& getResultFaces();

    private:
        int voxelCount = 0;
        std::vector<int> traversedCount;
        std::vector<int> notTraversedCount;
        std::vector<Point3D> resultVertices;
        std::vector<std::tuple<int, int, int>> resultFaces;
        void voxalizeRecursive(int depth, int maxDepth, Point3D pMin, Point3D pMax, std::vector<Point3D> &vertices, std::vector<std::tuple<int, int, int>> &faces, int startFaceIndex);
        bool isBetweenTwoPoints(Point3D p, Point3D min, Point3D max);
    };
}
#endif