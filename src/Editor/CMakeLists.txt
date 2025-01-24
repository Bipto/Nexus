cmake_minimum_required(VERSION 3.16)

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

nexus_copy_include_dir(project_template)
nexus_copy_glm(project_template)
nexus_copy_imgui(project_template)
nexus_copy_static_lib(project_template)
nexus_copy_required_binaries_to_custom_dir(project_template/libs)
nexus_copy_required_runtime_libraries_to_custom_dir(project_template/libs)