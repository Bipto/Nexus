cmake_minimum_required(VERSION 3.30)

project("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

#set up engine projects
add_subdirectory(src/BuildConfig)
add_subdirectory(src/Nexus)

if (DEFINED NX_BUILD_DEMO)
    add_subdirectory(src/Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()

if (DEFINED NX_BUILD_EDITOR)
    add_subdirectory(src/Editor)
endif()

if (DEFINED NX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/Tests)
endif()

target_include_directories(Nexus PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Nexus/include>
    $<INSTALL_INTERFACE:include>
)

install (TARGETS Nexus
    EXPORT NexusTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)

install (DIRECTORY include/
    DESTINATION include)