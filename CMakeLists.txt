cmake_minimum_required(VERSION 3.30)

project("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

set(NX_BUILD_TYPE SHARED)
add_compile_definitions(NX_BUILD_DLL=1)
add_compile_definitions(NX_EXPORT_API=1)

set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")

if (ANDROID)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${ANDROID_ABI})
endif()

#set up engine project
add_subdirectory(src/Nexus)

if (DEFINED WIN32)
    add_subdirectory(src/UITests)
endif()

if (DEFINED NX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/Tests)
  endif()

if (DEFINED NX_BUILD_DEMO)
    add_subdirectory(src/Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()

if (DEFINED NX_BUILD_EDITOR)
    add_subdirectory(src/Editor)
endif()