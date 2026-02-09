cmake_minimum_required(VERSION 3.28)

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

# Collect all runtime-producing dependencies recursively
function(nexus_collect_runtime_targets out_var target_name visited)
    # Avoid infinite recursion
    if(${target_name} IN_LIST visited)
        set(${out_var} "" PARENT_SCOPE)
        return()
    endif()

    list(APPEND visited ${target_name})

    set(result "")

    # Get direct dependencies
    get_target_property(deps ${target_name} LINK_LIBRARIES)

    foreach(dep IN LISTS deps)
        if(TARGET ${dep})
            get_target_property(dep_type ${dep} TYPE)
            get_target_property(is_imported ${dep} IMPORTED)

            # Only include SHARED or MODULE libraries
            if((dep_type STREQUAL "SHARED_LIBRARY"
                OR dep_type STREQUAL "MODULE_LIBRARY"
                OR (is_imported AND dep_type STREQUAL "UNKNOWN_LIBRARY")))
                list(APPEND result ${dep})
            endif()

            # Recurse into dependencies
            nexus_collect_runtime_targets(subdeps ${dep} "${visited}")
            list(APPEND result ${subdeps})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES result)
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

macro(nexus_copy_required_runtime_libraries target_name)

    if(NOT TARGET ${target_name})
        message(WARNING "Target '${target_name}' does not exist.")
        return()
    endif()

    # Collect recursive dependencies
    nexus_collect_runtime_targets(all_runtime_targets Nexus "")

    # Convert to generator expressions
    set(nexus_runtime_files "")
    foreach(dep IN LISTS all_runtime_targets)
        list(APPEND nexus_runtime_files $<TARGET_FILE:${dep}>)
    endforeach()

    # Copy all dependency DLLs
    if(nexus_runtime_files)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${nexus_runtime_files}
                $<TARGET_FILE_DIR:${target_name}>
            COMMAND_EXPAND_LISTS
            COMMENT "Copying recursive runtime DLLs for ${target_name}"
        )
    endif()

    # Copy Nexus.dll explicitly
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Nexus>
            $<TARGET_FILE_DIR:${target_name}>
        COMMENT "Copying Nexus.dll"
    )

    # Copy target's own runtime DLLs (CMake 3.21+)
    get_target_property(target_runtime_dlls ${target_name} RUNTIME_DLLS)
    if(target_runtime_dlls)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${target_name}>
                $<TARGET_FILE_DIR:${target_name}>
            COMMAND_EXPAND_LISTS
            COMMENT "Copying runtime DLLs for ${target_name}"
        )
    endif()

    # Copy PDB (MSVC only)
    if(MSVC)
        get_target_property(pdb_file Nexus PDB_OUTPUT_DIRECTORY)
        if(NOT pdb_file STREQUAL "pdb_file-NOTFOUND")
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_PDB_FILE:Nexus>
                    $<TARGET_FILE_DIR:${target_name}>
                COMMENT "Copying Nexus PDB"
            )
        endif()
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