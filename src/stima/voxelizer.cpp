#include "stima/voxelizer.hpp"
#include "stima/geometry.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace stima {
    volatile double Voxelizer::progress;
    volatile double Voxelizer::maxProgress;
    volatile double Voxelizer::lastPercentage;
    volatile int Voxelizer::threadCount;
    std::mutex Voxelizer::canOutput;
    int Voxelizer::voxelCount;
    std::vector<int> Voxelizer::traversedCount;
    std::vector<int> Voxelizer::notTraversedCount;
    std::vector<Point3D> Voxelizer::resultVertices;
    std::vector<std::tuple<int, int, int>> Voxelizer::resultFaces;
    void Voxelizer::voxelize(std::string inputFilePath, int maxDepth)
    {
        std::ifstream inputFile(inputFilePath);
        if (!inputFile.is_open()) {
            throw std::invalid_argument("Input path is not valid!");
        }
        std::filesystem::path filePath = inputFilePath;
        if (filePath.extension() != ".obj") {
            throw std::invalid_argument("Input file is not an object (.obj) file!");
        }

        std::vector<Point3D> vertices;
        std::vector<std::tuple<int, int, int>> faces;

        std::string line;
        std::string type;
        double x, y, z;
        int v1, v2, v3;
        while (std::getline(inputFile, line)) {
            std::istringstream lineStream(line);
            if (!(lineStream >> type)) {
                continue;
            }
            if (type == "v") {
                if (!(lineStream >> x >> y >> z)) {
                    continue;
                }
                vertices.push_back(Point3D(x, y, z));
            }
            else if (type == "f") {
                if (!(lineStream >> v1 >> v2 >> v3)) {
                    continue;
                }
                faces.push_back({v1, v2, v3});
            }
        }
        inputFile.close();

        Point3D pMin = vertices[0];
        Point3D pMax = vertices[0];
        for (auto &&p : vertices) {
            pMin.x = std::min(pMin.x, p.x);
            pMin.y = std::min(pMin.y, p.y);
            pMin.z = std::min(pMin.z, p.z);
            pMax.x = std::max(pMax.x, p.x);
            pMax.y = std::max(pMax.y, p.y);
            pMax.z = std::max(pMax.z, p.z);
        }
        Point3D v = pMax - pMin;
        double maxV = std::max(std::max(v.x, v.y), v.z);
        pMax = pMin + Point3D(maxV, maxV, maxV);
        progress = 0;
        maxProgress = (std::pow(8, maxDepth + 1) - 1) / 7.0;
        lastPercentage = 0;
        voxelCount = 0;
        traversedCount.clear();
        notTraversedCount.clear();
        resultVertices.clear();
        resultFaces.clear();
        auto start = std::chrono::steady_clock::now();
        voxalizeRecursive(0, maxDepth, pMin, pMax, vertices, faces, 0);
        auto end = std::chrono::steady_clock::now();
        std::cout << "Finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
        std::cout << "Outputting to " << "output/voxelized-" + filePath.filename().string() << std::endl;
        std::ofstream outputFile("output/voxelized-" + filePath.filename().string());
        for (auto &&p : resultVertices) {
            outputFile << "v " << p.x << " " << p.y << " " << p.z << "\n";
        }
        for (auto &&f : resultFaces) {
            outputFile << "f " << std::get<0>(f) << " " << std::get<1>(f) << " " << std::get<2>(f) << "\n";
        }
        outputFile.flush();
        outputFile.close();
        std::cout << "Finished!" << std::endl;
    }

    int Voxelizer::getVoxelCount()
    {
        return voxelCount;
    }
    const std::vector<int> &Voxelizer::getTraversedCount()
    {
        return traversedCount;
    }
    const std::vector<int> &Voxelizer::getNotTraversedCount()
    {
        return notTraversedCount;
    }
    const std::vector<Point3D> &Voxelizer::getResultVertices()
    {
        return resultVertices;
    }
    const std::vector<std::tuple<int, int, int>> &Voxelizer::getResultFaces()
    {
        return resultFaces;
    }
    void Voxelizer::voxalizeRecursive(int depth, int maxDepth, Point3D pMin, Point3D pMax, std::vector<Point3D> &vertices, std::vector<std::tuple<int, int, int>> &faces, int startFaceIndex)
    {
        // Permutasi titik-titik pada kubus
        static int kx[] = {0, 0, 0, 0, 1, 1, 1, 1};
        static int ky[] = {0, 0, 1, 1, 0, 0, 1, 1};
        static int kz[] = {0, 1, 0, 1, 0, 1, 0, 1};
        // Permutasi sisi
        static int fx[] = {1, 0, 0};
        static int fy[] = {0, 1, 0};
        static int fz[] = {0, 0, 1};
        // Tuple titik untuk membuat sisi kubus
        static int ki[6][4] = {
            {0, 1, 2, 3},
            {0, 1, 4, 5},
            {0, 2, 4, 6},
            {1, 3, 5, 7},
            {2, 3, 6, 7},
            {4, 5, 6, 7}};

        canOutput.lock();
        while ((int)traversedCount.size() <= depth) {
            traversedCount.push_back(0);
        }
        traversedCount[depth]++;
        canOutput.unlock();

        Point3D v = pMax - pMin;
        bool isIntersect = false;
        int intersectIndex = startFaceIndex;
        for (size_t f = startFaceIndex; f < faces.size(); f++) {
            // Point of triangle
            Point3D pt[3] = {
                vertices[std::get<0>(faces[f]) - 1],
                vertices[std::get<1>(faces[f]) - 1],
                vertices[std::get<2>(faces[f]) - 1]};
            // Check is one of the triangle point is inside the cube
            Plane3D triangleFace = Plane3D(pt[0], pt[1], pt[2]);
            for (int k = 0; k < 3; k++) {
                if (isBetweenTwoPoints(pt[k], pMin, pMax)) {
                    isIntersect = true;
                    break;
                }
            }
            if (isIntersect) {
                intersectIndex = f;
                break;
            }
            for (int j = 0; j < 6; j++) {
                // Point of square
                Point3D ps[4];
                if (j < 3) {
                    ps[0] = pMin;
                    ps[1] = pMin + Point3D(v.x * fx[j % 3], v.y * fy[j % 3], v.z * fz[j % 3]);
                    ps[2] = pMin + Point3D(v.x * fx[j % 3], v.y * fy[j % 3], v.z * fz[j % 3]) + Point3D(v.x * fx[(j + 1) % 3], v.y * fy[(j + 1) % 3], v.z * fz[(j + 1) % 3]);
                    ps[3] = pMin + Point3D(v.x * fx[(j + 1) % 3], v.y * fy[(j + 1) % 3], fz[(j + 1) % 3]);
                }
                else {
                    ps[0] = pMax - Point3D(v.x * fx[j % 3], v.y * fy[j % 3], v.z * fz[j % 3]) - Point3D(v.x * fx[(j + 1) % 3], v.y * fy[(j + 1) % 3], v.z * fz[(j + 1) % 3]);
                    ps[1] = pMax - Point3D(v.x * fx[j % 3], v.y * fy[j % 3], v.z * fz[j % 3]);
                    ps[2] = pMax;
                    ps[3] = pMax - Point3D(v.x * fx[(j + 1) % 3], v.y * fy[(j + 1) % 3], v.z * fz[(j + 1) % 3]);
                }
                // Check is one of the triangle side is intersecting the cube face
                Plane3D squareFace = Plane3D(ps[0], ps[1], ps[2]);
                for (int k = 0; k < 3; k++) {
                    Line3D triangleSide = Line3D(pt[k], pt[(k + 1) % 3]);
                    if (squareFace.isIntersect(triangleSide)) {
                        Point3D intersection = squareFace.getIntersection(triangleSide);
                        // std::cout << intersection.toString() << std::endl;
                        bool isInBetween = true;
                        if ((intersection - pt[k]).dotProduct(triangleSide.v) < 0 || (intersection - pt[(k + 1) % 3]).dotProduct(-triangleSide.v) < 0) {
                            isInBetween = false;
                        }
                        if (fx[j % 3] == 1 && fy[(j + 1) % 3] == 1) {
                            if (intersection.x < ps[0].x || intersection.x > ps[2].x) {
                                isInBetween = false;
                            }
                            if (intersection.y < ps[0].y || intersection.y > ps[2].y) {
                                isInBetween = false;
                            }
                        }
                        else if (fy[j % 3] == 1 && fz[(j + 1) % 3] == 1) {
                            if (intersection.y < ps[0].y || intersection.y > ps[2].y) {
                                isInBetween = false;
                            }
                            if (intersection.z < ps[0].z || intersection.z > ps[2].z) {
                                isInBetween = false;
                            }
                        }
                        else if (fz[j % 3] == 1 && fx[(j + 1) % 3] == 1) {
                            if (intersection.x < ps[0].x || intersection.x > ps[2].x) {
                                isInBetween = false;
                            }
                            if (intersection.z < ps[0].z || intersection.z > ps[2].z) {
                                isInBetween = false;
                            }
                        }
                        isIntersect = isInBetween;
                    }
                    if (isIntersect) {
                        break;
                    }
                }
                if (isIntersect) {
                    break;
                }
                // Check is one of the cube side (rusuk) is intersecting the triangle
                for (int k = 0; k < 4; k++) {
                    Line3D squareSide = Line3D(ps[k], ps[(k + 1) % 4]);
                    if (triangleFace.isIntersect(squareSide)) {
                        Point3D intersection = triangleFace.getIntersection(squareSide);
                        Point3D cp1 = (pt[1] - pt[0]).crossProduct(intersection - pt[0]).normalize();
                        Point3D cp2 = (pt[2] - pt[1]).crossProduct(intersection - pt[1]).normalize();
                        Point3D cp3 = (pt[0] - pt[2]).crossProduct(intersection - pt[2]).normalize();
                        if (cp1.isApproximately(cp2) && cp2.isApproximately(cp3) && cp3.isApproximately(cp1) &&
                            (intersection - ps[k]).dotProduct(squareSide.v) >= 0 && (intersection - ps[(k + 1) % 4]).dotProduct(-squareSide.v) >= 0) {
                            isIntersect = true;
                            break;
                        }
                    }
                }
                if (isIntersect) break;
            }

            if (isIntersect) {
                intersectIndex = f;
                break;
            }
        }

        if (isIntersect) {
            if (depth >= maxDepth) {
                canOutput.lock();
                voxelCount++;
                int startIndex = resultVertices.size() + 1;
                int &k = startIndex;
                Point3D v = pMax - pMin;
                for (int i = 0; i < 8; i++) {
                    Point3D q = pMin + Point3D(v.x * kx[i], v.y * ky[i], v.z * kz[i]);
                    resultVertices.push_back(q);
                }
                for (int i = 0; i < 6; i++) {
                    resultFaces.push_back({k + ki[i][0], k + ki[i][1], k + ki[i][2]});
                    resultFaces.push_back({k + ki[i][3], k + ki[i][1], k + ki[i][2]});
                }
                canOutput.unlock();
            }
            else {
                Point3D v = (pMax - pMin) / 2;
                Point3D q1[8];
                Point3D q2[8];
                for (int i = 0; i < 8; i++) {
                    q1[i] = pMin + Point3D(v.x * kx[i], v.y * ky[i], v.z * kz[i]);
                    q2[i] = q1[i] + v;
                }
                if (depth >= 3) {
                    voxalizeRecursive(depth + 1, maxDepth, q1[0], q2[0], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[1], q2[1], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[2], q2[2], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[3], q2[3], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[4], q2[4], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[5], q2[5], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[6], q2[6], std::ref(vertices), std::ref(faces), intersectIndex);
                    voxalizeRecursive(depth + 1, maxDepth, q1[7], q2[7], std::ref(vertices), std::ref(faces), intersectIndex);
                }
                else {
                    std::thread t1(voxalizeRecursive, depth + 1, maxDepth, q1[0], q2[0], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t2(voxalizeRecursive, depth + 1, maxDepth, q1[1], q2[1], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t3(voxalizeRecursive, depth + 1, maxDepth, q1[2], q2[2], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t4(voxalizeRecursive, depth + 1, maxDepth, q1[3], q2[3], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t5(voxalizeRecursive, depth + 1, maxDepth, q1[4], q2[4], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t6(voxalizeRecursive, depth + 1, maxDepth, q1[5], q2[5], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t7(voxalizeRecursive, depth + 1, maxDepth, q1[6], q2[6], std::ref(vertices), std::ref(faces), intersectIndex);
                    std::thread t8(voxalizeRecursive, depth + 1, maxDepth, q1[7], q2[7], std::ref(vertices), std::ref(faces), intersectIndex);
                    t1.join();
                    t2.join();
                    t3.join();
                    t4.join();
                    t5.join();
                    t6.join();
                    t7.join();
                    t8.join();
                }
            }
        }
        else {
            canOutput.lock();
            while ((int)notTraversedCount.size() <= depth)
                notTraversedCount.push_back(0);
            notTraversedCount[depth]++;
            canOutput.unlock();
            progress += (std::pow(8, maxDepth - depth + 1) - 1) / 7.0;
            // if (progress / maxProgress > lastPercentage) {
            // std::cout << "Progress (" << std::round(progress / maxProgress * 10) / 10 << "%)..." << std::endl;
            //     lastPercentage = std::ceil(progress / maxProgress);
            // }
        }
    }
    bool Voxelizer::isBetweenTwoPoints(Point3D p, Point3D min, Point3D max)
    {
        return min.x <= p.x && p.x <= max.x && min.y <= p.y && p.y <= max.y && min.z <= p.z && p.z <= max.z;
    }
}