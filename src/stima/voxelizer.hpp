#ifndef STIMA_VOXELIZER_H
#define STIMA_VOXELIZER_H
#include "stima/geometry.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <mutex>

namespace stima {
    class Voxelizer {
    public:
        void voxelize(std::string inputFilePath, int maxDepth);
        int getVoxelCount();
        const std::vector<int> &getTraversedCount();
        const std::vector<int> &getNotTraversedCount();
        const std::vector<Point3D> &getResultVertices();
        const std::vector<std::tuple<int, int, int>> &getResultFaces();

    private:
        static volatile double progress;
        static volatile int threadCount;
        static volatile double maxProgress;
        static volatile double lastPercentage;
        static std::mutex canOutput;
        static int voxelCount;
        static std::vector<int> traversedCount;
        static std::vector<int> notTraversedCount;
        static std::vector<Point3D> resultVertices;
        static std::vector<std::tuple<int, int, int>> resultFaces;
        static void voxalizeRecursive(int depth, int maxDepth, Point3D pMin, Point3D pMax, std::vector<Point3D> &vertices, std::vector<std::tuple<int, int, int>> &faces, int startFaceIndex);
        static bool isBetweenTwoPoints(Point3D p, Point3D min, Point3D max);
    };
}
#endif