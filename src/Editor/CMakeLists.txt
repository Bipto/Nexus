cmake_minimum_required(VERSION 3.28)

project("Editor")

add_executable(Editor main.cpp)
target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()
setup_scripting_project(Nexus)