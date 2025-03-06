cmake_minimum_required(VERSION 3.30)

macro (nexus_copy_resources)
  #copy engine resources to output
  get_target_property(Nexus_SOURCE_DIR Nexus SOURCE_DIR)

  add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${Nexus_SOURCE_DIR}/resources/engine
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/engine)
endmacro()

macro(nexus_copy_required_binaries)
  if (WIN32)
    get_target_property(Nexus_SOURCE_DIR Nexus SOURCE_DIR)

    add_custom_command(
      TARGET ${PROJECT_NAME} PRE_LINK
      COMMAND ${CMAKE_COMMAND} -E copy
          ${Nexus_SOURCE_DIR}/binaries/dxc_2023_08_14/bin/x64/dxcompiler.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )

  add_custom_command(
      TARGET ${PROJECT_NAME} PRE_LINK
      COMMAND ${CMAKE_COMMAND} -E copy
          ${Nexus_SOURCE_DIR}/binaries/dxc_2023_08_14/bin/x64/dxil.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
  endif()
endmacro()

macro(nexus_copy_required_binaries_to_custom_dir output_dir)
  if (WIN32)
    get_target_property(Nexus_SOURCE_DIR Nexus SOURCE_DIR)

    #create directory if needed
    add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
      COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir}  
)

    add_custom_command(
      TARGET ${PROJECT_NAME} PRE_LINK
      COMMAND ${CMAKE_COMMAND} -E copy
          ${Nexus_SOURCE_DIR}/binaries/dxc_2023_08_14/bin/x64/dxcompiler.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir}
  )

  add_custom_command(
      TARGET ${PROJECT_NAME} PRE_LINK
      COMMAND ${CMAKE_COMMAND} -E copy
          ${Nexus_SOURCE_DIR}/binaries/dxc_2023_08_14/bin/x64/dxil.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir}
    )
  endif()
endmacro()


macro(nexus_add_shared_library shared_library)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  add_definitions(-DLIBTYPE=SHARED)
  add_subdirectory(${shared_library})
  remove_definitions(-DLIBTYPE)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endmacro()

macro(nexus_setup_filters source_files)
  foreach(_source IN ITEMS ${source_files})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
  endforeach()
endmacro()

macro(nexus_export_symbols)
  get_all_targets(all_targets)

  foreach (_target IN LISTS all_targets)
    if (NOT _target STREQUAL "Nexus")
      set_property(TARGET ${_target} PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS true)
    endif()
  endforeach()
  
endmacro()

function(get_all_targets var)
  set(targets)
  get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
  set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
  get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
  foreach(subdir ${subdirectories})
    get_all_targets_recursive(${targets} ${subdir})
  endforeach()

  get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${targets} ${current_targets})
endmacro()

macro(nexus_structure_folders)
  get_all_targets(all_targets)
  foreach (_target IN LISTS all_targets)
    if (NOT _target STREQUAL "Nexus")
      set_target_properties(${_target} PROPERTIES FOLDER "external")
    endif()
  endforeach()
endmacro()

#copy all required libraries to output directory
macro(nexus_copy_required_runtime_libraries)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS)

  #this is only required if we are building a shared library
  get_target_property(TARGET_TYPE Nexus TYPE)
  if (TARGET_TYPE STREQUAL "SHARED_LIBRARY")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_FILE:Nexus>
      COMMAND_EXPAND_LISTS)
  endif()
endmacro()

macro(nexus_copy_required_runtime_libraries_to_custom_dir output_dir)

  #create directory if needed
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir}  
  )

  #copy libraries
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir} $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS)

endmacro()

macro(nexus_copy_required_runtime_libraries_to_target_output target)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  -t $<TARGET_FILE_DIR:${target}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
  COMMAND_EXPAND_LISTS)
endmacro()

macro(nexus_copy_static_lib output_dir)

  #create directory if needed
  add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir}  
  )

  #copy libraries
  add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}>/${output_dir} $<TARGET_FILE:Nexus>
    COMMAND_EXPAND_LISTS)

endmacro()

macro(nexus_add_all_subdirs SUBDIRS)
  foreach (SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR})
  endforeach()
endmacro()

macro(nexus_add_all_shared_lib_subdirs SUBDIRS)
  foreach (SUBDIR ${SUBDIRS})
    nexus_add_shared_library(${SUBDIR})
  endforeach()
endmacro()

macro(setup_scripting_project output_dir)

  set(NEXUS_OUTPUT_INCLUDE_DIR ${output_dir}/include)
  set(NEXUS_OUTPUT_LIB_DIR ${output_dir}/lib)

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_INCLUDE_DIR}
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_LIB_DIR}
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_LIB_DIR} $<TARGET_FILE:Nexus>
    COMMAND_EXPAND_LISTS)

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_LIB_DIR} $<TARGET_LINKER_FILE:Nexus>
    COMMAND_EXPAND_LISTS)

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${Nexus_SOURCE_DIR}/include
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_INCLUDE_DIR})

endmacro()