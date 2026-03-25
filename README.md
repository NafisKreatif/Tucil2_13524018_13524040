# 3D Object Voxelization - Tugas Kecil 2 IF2211 Strategi Algoritma 2026

## Description

Program ini melakukan proses <i>voxelization</i> pada sebuah objek 3 dimensi dalam file `.obj`, yaitu mengubah objek tersebut menjadi objek yang terdiri dari kubus-kubus kecil. Proses <i>voxelization</i> dilakukan dengan strategi algoritma <i>Divide and Conquer</i>. Kubus yang lebih besar dipecah menjadi 8 kubus yang lebih kecil. 

Program dibuat dalam bahasa C++ dan dipercepat dengan melakukan multithreading. Terdapat juga program renderer untuk melihat objek 3d dari file .obj.

## Requirements

- **C++17 or above** — Required for modern C++ standard library features
- **SFML 2.6.2** — Simple and Fast Multimedia Library for graphics rendering
- **GNU Make** — Build automation tool (comes with MinGW/GCC on Windows, `make` or `cmake` on Linux)
- **g++** — GNU C++ compiler (part of MinGW-w64 on Windows, GCC on Linux)

Direkomendasikan menggunakan Windows Subsystem for Linux (WSL) 

## Installation

### 1. Install C++ Compiler and Build Tools

#### Windows (MSYS2 + MinGW-w64)
```bash
# Download dan install MSY2 dari https://www.msys2.org/
# Buka terminal MSYS2 MinGW 64-bit dan jalankan:
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential g++ make cmake
```

### 2. Install SFML

#### Windows (MSYS2 + MinGW-w64)
```bash
pacman -S mingw-w64-x86_64-sfml
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt install libsfml-dev
```

### 3. Build and Run

#### Main Program
```bash
# Clean previous builds (optional)
make clean-all

# Build main executable
make all

# Run the program
make run
# or
./bin/main
```

```bash
# Build renderer
make renderer

# Run renderer
./bin/renderer path/to/object.obj
```

## Author

- Muhammad Nafis Habibi - 13524018 - @NafisKreatif
- Kloce Paul William Saragih - 13424040 - @YeyThePotatoMan
