cmake_minimum_required(VERSION 3.16)

project("Demo")

if (ANDROID OR __ANDROID__)
    add_library(Demo SHARED main.cpp)
else()
    add_executable(Demo src/main.cpp)
endif()

target_include_directories(Demo PUBLIC ../Nexus/src)
target_link_libraries(Demo PUBLIC Nexus)

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Demo
        PROPERTIES
        LINK_FLAGS
        "--std=c++20 -fsanitize=address -sSTACK_SIZE=5MB -Os -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sNO_DISABLE_EXCEPTION_CATCHING -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -lopenal -sGL_ENABLE_GET_PROC_ADDRESS -sASSERTIONS --preload-file resources"
    )

    set_target_properties(Nexus PROPERTIES COMPILE_FLAGS "-Os")
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

    #copy all required libraries to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS)


endif()

#set the working directory to the executable directory
#this ONLY applies to Visual Studio
set_property(TARGET Demo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)

nexus_copy_required_binaries()