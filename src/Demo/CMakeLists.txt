cmake_minimum_required(VERSION 3.30)

set(NX_BUILD_SHARED ON)
project("Demo")

if (ANDROID OR __ANDROID__)
add_library(Demo SHARED main.cpp)
else()
add_executable(Demo src/main.cpp)
endif()

target_include_directories(Demo PUBLIC ../Nexus/src)
target_link_libraries(Demo PUBLIC Nexus)

if (EMSCRIPTEN)
    set_target_properties(
        Demo
            PROPERTIES
            LINK_FLAGS
            "--std=c++20 -O3 -flto -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS --preload-file resources"
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
    add_custom_command(
    TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/html
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    #copy resources folder to output directory
    add_custom_command(
    TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources)

else()
    #copy resources folder to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
endif()

#set the working directory to the executable directory and startup project to the Demo project
#this ONLY applies to Visual Studio
set_property(TARGET Demo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()