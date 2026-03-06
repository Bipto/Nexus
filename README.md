# Nexus

[![Build and Test (Multi‑Platform)](https://github.com/Bipto/Nexus/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/Bipto/Nexus/actions/workflows/build_and_test.yml)

Nexus is a cross-platform 2D and 3D graphics and audio library, encompassing a Render Hardware Interface (RHI) across OpenGL, Vulkan and D3D12 and various utilities to support resource loading. It provides a clean and simple abstraction layer across all supported API's while still providing low level control and optimization opportunities for performance-critical applications.

<img width="1277" height="757" alt="image" src="https://github.com/user-attachments/assets/407d6a9d-a67c-4bb0-af08-add8a71bc4da" />

# Features
- Unified abstraction for buffers, textures, resource sets, devices, queues, command lists, and acceleration structures
-  Automatic layout and barrier management (toggleable per command list)
-  Support for graphics, meshlet, and compute pipelines
-  Extension/version handling for OpenGL and Vulkan based on driver capabilities
-  Integrated custom ImGui backend implementation
-  Intuitive API design with low-level access preserved

# Building

Windows
```cmd
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
```

Linux
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```

The project uses CMake to build and vcpkg to manage dependencies. Presets are available for Windows and Linux builds across multiple compilers and build configurations. The recommended way to use vcpkg is by setting the VCPKG_ROOT environment variable. This is used by all presets.

Required CMake variables:
- CMAKE_TOOLCHAIN_FILE: The path to vcpkg's toolchain file for importing and managing dependencies

Optional CMake variables:
- NX_PLATFORM_OPENGL: Builds the OpenGL graphics implementation
- NX_PLATFORM_VULKAN: Builds the Vulkan graphics implementation
- NX_PLATFORM_D3D12: Builds the DirectD3D12 graphics implementation
- NX_PLATFORM_OPENAL: Builds the OpenAL audio implementation
- NX_BUILD_DEMO: Builds the demo suite application
- NX_BUILD_EDITOR: Builds the experimental level editor
- NX_BUILD_TESTS: Builds unit tests

# Demos
Repository contains a full demo suite, containing examples of all supported features, including command recording, indirect rendering, model loading, lighting and audio.

<img width="1278" height="756" alt="495122494-cc79f8c6-0865-4c01-8fde-e97ab20738cf" src="https://github.com/user-attachments/assets/f5c94610-317c-4c98-8a21-43d9794e1209" />

<img width="1532" height="913" alt="image" src="https://github.com/user-attachments/assets/30cf174c-f9c0-4a78-beb9-0e45f7615f72" />

<img width="1476" height="951" alt="image" src="https://github.com/user-attachments/assets/55dbb00e-800f-4715-af3e-fdfbe165e9e8" />
