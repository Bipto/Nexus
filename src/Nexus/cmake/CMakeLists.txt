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

    # ============================================================
    # 1. Collect DLLs CMake *knows* about (IMPORTED targets)
    # ============================================================
    set(_KNOWN_DLLS "$<TARGET_RUNTIME_DLLS:${target_name}>")

    # ============================================================
    # 2. Collect all vcpkg DLLs (debug + release)
    # ============================================================
    file(GLOB _VCPKG_ALL_DLLS
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll"
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin/*.dll"
    )

    # ============================================================
    # 3. Filter vcpkg DLLs by actual link dependencies
    # ============================================================
    get_target_property(_LINKED_LIBS ${target_name} LINK_LIBRARIES)

    set(_FILTERED_VCPKG_DLLS "")
    foreach(dll ${_VCPKG_ALL_DLLS})
        get_filename_component(_dll_name "${dll}" NAME_WE)

        foreach(lib ${_LINKED_LIBS})
            # Match DLL name to library name (case-insensitive)
            string(TOLOWER "${lib}" _lib_lower)
            string(TOLOWER "${_dll_name}" _dll_lower)

            if(_lib_lower MATCHES "${_dll_lower}")
                list(APPEND _FILTERED_VCPKG_DLLS "${dll}")
            endif()
        endforeach()
    endforeach()

    # ============================================================
    # 4. MinGW runtime DLLs (if needed)
    # ============================================================
    if(MINGW)
        get_filename_component(_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

        file(GLOB _MINGW_RUNTIME_DLLS
            "${_MINGW_BIN_DIR}/libgcc_s_*.dll"
            "${_MINGW_BIN_DIR}/libstdc++-6.dll"
            "${_MINGW_BIN_DIR}/libwinpthread-1.dll"
        )
    endif()

    # ============================================================
    # 5. Copy everything relevant
    # ============================================================
    set(_ALL_DLLS
        ${_KNOWN_DLLS}
        ${_FILTERED_VCPKG_DLLS}
        ${_MINGW_RUNTIME_DLLS}
    )

    # Remove empty entries
    list(FILTER _ALL_DLLS EXCLUDE REGEX "^$")

    if(WIN32 AND _ALL_DLLS)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${_ALL_DLLS}
                $<TARGET_FILE_DIR:${target_name}>
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