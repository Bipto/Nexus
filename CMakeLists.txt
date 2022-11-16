cmake_minimum_required(VERSION 3.16)

project("NexusEngine")

add_subdirectory(Application)
add_subdirectory(NexusEngine)

add_subdirectory(External/SDL)
#add_subdirectory(External/Lua)
add_subdirectory(External/glad)

#Editor
target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)

if (EMSCRIPTEN)
    set_target_properties(
        Editor
        PROPERTIES
        LINK_FLAGS
        "-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 -s FULL_ES3=1"
    )
endif()