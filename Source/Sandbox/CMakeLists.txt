cmake_minimum_required(VERSION 3.16)

project("Sandbox")

add_executable(Sandbox main.cpp)

target_include_directories(Sandbox PUBLIC NexusEngine)
target_link_libraries(Sandbox PUBLIC NexusEngine)

#copy resources folder to output directory
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources)

#copy all required libraries to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)

#set the working directory to the executable directory
set_property(TARGET Demo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

#copy required DXC libraries to executable directory
if (WIN32)
  add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_SOURCE_DIR}/Binaries/dxc_2023_08_14/bin/x64/dxcompiler.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
  add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_SOURCE_DIR}/Binaries/dxc_2023_08_14/bin/x64/dxil.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()