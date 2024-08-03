cmake_minimum_required(VERSION 3.16)

project("Editor")

file (GLOB_RECURSE APP_SOURCES 
    main.cpp
    UI/*.cpp
    UI/Dialogs/*.cpp)

add_executable(Editor main.cpp ${APP_SOURCES})

target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus)

target_include_directories(Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#copy resources folder to output directory
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources)

#copy all required libraries to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

nexus_copy_required_binaries()