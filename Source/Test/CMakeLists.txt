cmake_minimum_required(VERSION 3.16)

project("Test")

add_executable(Test main.cpp)

target_include_directories(Test PUBLIC NexusEngine)
target_link_libraries(Test PUBLIC NexusEngine)

#copy resources folder to output directory
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    ${CMAKE_CURRENT_BINARY_DIR}/Resources)

#copy all required libraries to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
  )