cmake_minimum_required(VERSION 3.16)

project("Editor")

add_executable(Editor main.cpp)
target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus)

#copy all required libraries to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy
-t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
COMMAND_EXPAND_LISTS)

nexus_copy_required_binaries()