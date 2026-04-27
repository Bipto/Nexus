file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.hpp
    src/Demos/*
    src/Layers/*
)

if (ANDROID OR __ANDROID__)
add_library(Demo SHARED ${SOURCE_FILES})
else()
add_executable(Demo ${SOURCE_FILES})
endif()

target_include_directories(Demo PUBLIC ../Nexus/src)
target_link_libraries(Demo PUBLIC Nexus)

target_link_libraries(Demo PUBLIC Nexus-UI Nexus-UI-ImGui)

if (EMSCRIPTEN)
    set_target_properties(
        Demo
            PROPERTIES
            LINK_FLAGS
            "--std=c++20 -O3 -flto -sALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS --preload-file resources"
        )
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lidbfs.js")
        target_link_options(Demo PRIVATE -lidbfs.js)
endif()

if (ANDROID OR __ANDROID__)
    target_link_libraries(
        Demo
        PUBLIC
        EGL
        GLESv2
        GLESv3
    )
endif()

if (EMSCRIPTEN)
    copy_directory_to_target_output(Nexus html)
    copy_directory_to_target_output(Demo resources)
else()
    #copy resources folder to output directory
    copy_directory_to_target_output(Demo resources)
endif()

#set the working directory to the executable directory and startup project to the Demo project
#this ONLY applies to Visual Studio
foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
    set_property(TARGET Demo PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY_${cfg}
        "${CMAKE_CURRENT_BINARY_DIR}/${cfg}"
    )
endforeach()

copy_runtime_deps(Demo)

if (ANDROID)
    set_target_properties(Demo PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif()
