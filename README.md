# Nexus

[![Build (Windows)](https://github.com/Bipto/Nexus/actions/workflows/build-windows.yml/badge.svg)](https://github.com/Bipto/Nexus/actions/workflows/build-windows.yml)
[![Build (Linux)](https://github.com/Bipto/Nexus/actions/workflows/build-linux.yml/badge.svg)](https://github.com/Bipto/Nexus/actions/workflows/build-linux.yml)
[![CodeQL](https://github.com/Bipto/Nexus/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/Bipto/Nexus/actions/workflows/github-code-scanning/codeql)

Nexus is a cross-platform 2D and 3D graphics and audio library, encompassing a Render Hardware Interface (RHI) across OpenGL, Vulkan and D3D12 and various utilities to support resource loading. It provides a clean and simple abstraction layer across all supported API's while still providing low level control and optimization opportunities for performance-critical applications.

<img width="1277" height="757" alt="image" src="https://github.com/user-attachments/assets/407d6a9d-a67c-4bb0-af08-add8a71bc4da" />

# Features
- Unified abstraction for buffers, textures, resource sets, devices, queues, command lists, and acceleration structures
-  Automatic layout and barrier management (toggleable per command list)
-  Support for graphics, meshlet, ray-tracing and compute pipelines
-  Extension/version handling for OpenGL and Vulkan based on driver capabilities
-  Integrated custom ImGui backend implementation
-  Intuitive API design with low-level access preserved

# Architecture
The project is split into multiple components including Audio, Core, Engine, Platform and RHI. Core contains core functionality needed by all other modules. Engine is the top level module and contains all of the functionality you will need to build custom applications. All other modules have limited dependencies on each other. All modules have their own src and include directories and include their own unit tests if relevant.

# Building

```cmd
cmake -S . -B build
```

The project uses CMake to build and CPM.cmake to manage dependency management and build reproducibility. Presets are available for Windows and Linux builds across multiple compilers and build configurations.

Optional CMake variables:
- NX_PLATFORM_OPENGL: Builds the OpenGL graphics implementation
- NX_PLATFORM_VULKAN: Builds the Vulkan graphics implementation
- NX_PLATFORM_D3D12: Builds the Direct3D12 graphics implementation
- NX_PLATFORM_OPENAL: Builds the OpenAL audio implementation
- NX_BUILD_DEMO: Builds the demo suite application
- NX_BUILD_EDITOR: Builds the experimental level editor
- BUILD_TESTING: Builds unit tests

# Documentation
Documentation of source code is generated automatically through Doxygen with every commit. Documentation is available through a browser at:  https://bipto.github.io/Nexus or as a PDF document as a GitHub Actions artifact.

# Testing
Testing is done using googletest. Unit tests are built and executed as part of CI/CD workflows through GitHub Actions. Each module of the project is capable of having their own unit tests, although for some that will not be possible, e.g. running graphics tests is not possible due to limited GPU driver support with GitHub runners.

# Demos
Repository contains a full demo suite, containing examples of all supported features, including command recording, indirect rendering, model loading, lighting and audio.

<img width="1278" height="756" alt="495122494-cc79f8c6-0865-4c01-8fde-e97ab20738cf" src="https://github.com/user-attachments/assets/f5c94610-317c-4c98-8a21-43d9794e1209" />

<img width="1532" height="913" alt="image" src="https://github.com/user-attachments/assets/30cf174c-f9c0-4a78-beb9-0e45f7615f72" />

<img width="1278" height="756" alt="ezgif-8fbc1b6ca8851061" src="https://github.com/user-attachments/assets/83351e91-e93f-4a17-97a9-d9c7c5078a5d" />
