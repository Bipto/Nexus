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

macro(copy_runtime_deps target_name)
    if(NOT TARGET ${target_name})
        message(WARNING "Target '${target_name}' does not exist.")
        return()
    endif()

    # Detect whether this CMake supports get_runtime_dependencies
    set(_HAS_GET_RUNTIME_DEPS FALSE)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
        # VS ships a custom CMake that reports a fake version but lacks the command.
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E help
            OUTPUT_VARIABLE _CMAKE_E_HELP
        )
        if(_CMAKE_E_HELP MATCHES "get_runtime_dependencies")
            set(_HAS_GET_RUNTIME_DEPS TRUE)
        endif()
    endif()

    set(_OUT_DIR $<TARGET_FILE_DIR:${target_name}>)

    if(_HAS_GET_RUNTIME_DEPS)
        message(STATUS "Using get_runtime_dependencies for ${target_name}")

        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_OUT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E env
                CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM=windows
                "${CMAKE_COMMAND}" -E get_runtime_dependencies
                    --executable $<TARGET_FILE:${target_name}>
                    --library-dirs "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin"
                    --library-dirs "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin"
                    --copy "${_OUT_DIR}"
            VERBATIM
        )

    else()
        message(WARNING
            "CMake does not support get_runtime_dependencies here. "
            "Falling back to TARGET_RUNTIME_DLLS for ${target_name}."
        )

        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${target_name}>
                "${_OUT_DIR}"
            COMMAND_EXPAND_LISTS
            VERBATIM
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