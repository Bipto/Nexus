add_executable(Editor main.cpp)
target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus)

add_custom_command(TARGET Editor POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts
    $<TARGET_FILE_DIR:Editor>/fonts)

copy_runtime_deps(Editor)