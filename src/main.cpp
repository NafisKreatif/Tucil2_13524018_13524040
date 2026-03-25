#include "stima/voxelizer.hpp"
#include <iostream>
using namespace std;

int main()
{
    cout << "Input the .obj path: ";
    string path;
    getline(cin, path);
    cout << "Input max depth: ";
    int maxDepth;
    cin >> maxDepth;

    stima::Voxelizer voxelizer = stima::Voxelizer();
    try {
        voxelizer.voxelize(path, maxDepth);
        cout << "STATISTICS" << endl;
        cout << "=========================" << endl;
        cout << "Voxel Count: " << voxelizer.getVoxelCount() << endl;
        cout << "Vertice Count: " << voxelizer.getResultVertices().size() << endl;
        cout << "Face (Triangle) Count: " << voxelizer.getResultFaces().size() << endl;
        cout << "Traversed Node in Recursion Tree: " << endl;
        auto &traversedCount = voxelizer.getTraversedCount();
        for (size_t i = 0; i < traversedCount.size(); i++) {
            cout << "Depth " << i << ": " << traversedCount[i] << endl;
        }
        cout << "Killed Node in Recursion Tree: " << endl;
        auto &notTraversedCount = voxelizer.getNotTraversedCount();
        for (size_t i = 0; i < notTraversedCount.size(); i++) {
            cout << "Depth " << i << ": " << notTraversedCount[i] << endl;
        }
    }
    catch (const exception &e) {
        cout << "Gagal melakukan voxelisasi..." << endl;
        cout << e.what() << endl;
    }

    return 0;
}