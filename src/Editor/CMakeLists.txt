file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.hpp
)

add_executable(Editor ${SOURCE_FILES})

target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus Nexus-UI Nexus-UI-wxWidgets)

set_target_properties(Editor PROPERTIES
    WIN32_EXECUTABLE TRUE
)

#set the working directory to the executable directory and startup project to the Demo project
#this ONLY applies to Visual Studio
foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
    set_property(TARGET Editor PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY_${cfg}
        "${CMAKE_CURRENT_BINARY_DIR}/${cfg}"
    )
endforeach()

copy_runtime_deps(Editor)