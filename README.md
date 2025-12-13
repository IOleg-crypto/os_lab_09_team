# OS_9 with team

A lightweight, cross-platform (Windows) C++ application template using **Dear ImGui**, **GLFW**, and **OpenGL 3.3 **. This project features a custom borderless, transparent window setup and organized architecture using manager classes.

## Features

- **Cross-Platform**: Runs on Windows.
- **Custom Window Style**: Borderless, transparent, and layered window support.
- **Architecture**: Clean separation of concerns with `Application`, `WindowManager`, and `GuiRenderer`.
- **GPU Preference**: Automatically requests high-performance dedicated GPU (Nvidia/AMD).
- **ImGui Integration**: configured with a dark theme and custom styling examples.

## Prerequisites

- **CMake** (3.20 or newer)
- **C++ Compiler** with C++20 support (Clang)
- **Git**

## Building the Project

### Windows (PowerShell/Git Bash)

1. **Clone the repository** (ensure submodules are initialized):

   **Using HTTPS:**

   ```powershell
   git clone --recursive https://github.com/IOleg-crypto/os_lab_09_team.git
   cd os_lab_09_team
   ```

   **Using SSH:**

   ```powershell
   git clone --recursive git@github.com:IOleg-crypto/os_lab_09_team.git
   cd os_lab_09_team
   ```

2. **Configure with CMake** (using Ninja):

   ```powershell
   cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -B build -S .
   ```

3. **Build**:

   ```powershell
   cmake --build build
   ```

4. **Run**:
   ```powershell
   .\build\os_9.exe
   ```
