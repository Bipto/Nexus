cmake_minimum_required(VERSION 3.28)

macro(nexus_copy_required_binaries)
  if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy
              ${CMAKE_SOURCE_DIR}/src/Nexus/external/dxc_2025_07_14/bin/x64/dxcompiler.dll
              $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    add_custom_command(
      TARGET ${PROJECT_NAME} PRE_LINK
      COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/src/Nexus/external/winpixeventruntime.1.0.240308001/bin/x64/WinPixEventRuntime.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
  endif()
endmacro()

macro(nexus_setup_filters source_files)
  foreach(_source IN ITEMS ${source_files})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
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

macro(nexus_copy_required_runtime_libraries)

    set(target_name ${PROJECT_NAME})

    # Copy runtime DLLs (Windows-specific)
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${target_name}>
                $<TARGET_FILE_DIR:${target_name}>
        COMMAND_EXPAND_LISTS
        COMMENT "Post-build: Copying runtime DLLs for ${target_name}"
    )

    # Copy PDB file if it exists (Windows/MSVC)
    get_target_property(pdb_file ${target_name} PDB_OUTPUT_DIRECTORY)

    if(NOT pdb_file STREQUAL "pdb_file-NOTFOUND")
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_PDB_FILE:Nexus>
                    $<TARGET_FILE_DIR:${target_name}>
            COMMENT "Post-build: Copying PDB file for ${target_name}"
        )
    endif()

endmacro()



macro(nexus_add_all_subdirs SUBDIRS)
  foreach (SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR} EXCLUDE_FROM_ALL)
  endforeach()
endmacro()

macro(setup_scripting_project output_dir)

  set(NEXUS_OUTPUT_INCLUDE_DIR ${output_dir}/include)
  set(NEXUS_OUTPUT_LIB_DIR ${output_dir}/lib)

  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_INCLUDE_DIR}
  )

  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_LIB_DIR}
  )

  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_LIB_DIR} $<TARGET_FILE:Nexus>
    COMMAND_EXPAND_LISTS)

  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    -t $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_LIB_DIR} $<TARGET_LINKER_FILE:Nexus>
    COMMAND_EXPAND_LISTS)

  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${Nexus_SOURCE_DIR}/include
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/${NEXUS_OUTPUT_INCLUDE_DIR})

endmacro()