file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.hpp
)

add_executable(Editor ${SOURCE_FILES})

target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus)

CPMAddPackage(
	NAME wxWidgets
	GITHUB_REPOSITORY wxWidgets/wxWidgets
	GIT_TAG v3.3.2
        "wxBUILD_SHARED OFF"
        "wxBUILD_TESTS OFF"
)

target_link_libraries(Editor PRIVATE wx::net wx::core wx::base wx::aui)

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
