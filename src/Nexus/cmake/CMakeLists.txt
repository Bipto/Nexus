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

    if(NOT MSVC)
        return()
    endif()

    # Embedded script
    set(_copy_script "
        file(MAKE_DIRECTORY \"\${OUT_DIR}\")

        # Copy target PDB
        if(EXISTS \"\${TARGET_PDB}\")
            file(COPY \"\${TARGET_PDB}\" DESTINATION \"\${OUT_DIR}\")
        endif()

        # Determine vcpkg dirs based on config
        if(\"\${CONFIG}\" STREQUAL \"Debug\")
            set(VCPKG_BIN \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin\")
            set(VCPKG_LIB \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib\")
        else()
            set(VCPKG_BIN \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin\")
            set(VCPKG_LIB \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib\")
        endif()

        # Glob vcpkg DLLs
        file(GLOB VCPKG_DLLS \"\${VCPKG_BIN}/*.dll\")

        # Merge DLL sources
        set(ALL_DLLS \${VCPKG_DLLS} \${RUNTIME_DLLS})

        foreach(d IN LISTS EXTRA_DLL_DIRS)
            file(GLOB EXTRA_DLLS \"\${d}/*.dll\")
            list(APPEND ALL_DLLS \${EXTRA_DLLS})
        endforeach()

        # Helper: find matching PDB
        function(find_pdb dll_path out_var)
            get_filename_component(name_we \"\${dll_path}\" NAME_WE)
            get_filename_component(dll_dir \"\${dll_path}\" DIRECTORY)

            set(names \"\${name_we}.pdb\" \"\${name_we}d.pdb\")
            set(dirs \"\${dll_dir}\" \"\${VCPKG_LIB}\" \"\${VCPKG_BIN}\" \"\${TARGET_DIR}\")

            foreach(d IN LISTS dirs)
                foreach(n IN LISTS names)
                    set(candidate \"\${d}/\${n}\")
                    if(EXISTS \"\${candidate}\")
                        set(\${out_var} \"\${candidate}\" PARENT_SCOPE)
                        return()
                    endif()
                endforeach()
            endforeach()

            set(\${out_var} \"\" PARENT_SCOPE)
        endfunction()

        # Copy DLLs + PDBs
        foreach(dll IN LISTS ALL_DLLS)
            file(COPY \"\${dll}\" DESTINATION \"\${OUT_DIR}\")

            find_pdb(\"\${dll}\" pdb_path)
            if(pdb_path)
                file(COPY \"\${pdb_path}\" DESTINATION \"\${OUT_DIR}\")
            endif()
        endforeach()
    ")

    # Write script
    set(_script_path "${CMAKE_BINARY_DIR}/copy_runtime_deps_${target_name}.cmake")
    file(WRITE "${_script_path}" "${_copy_script}")

    # Add post-build step
    add_custom_command(
        TARGET ${target_name} POST_BUILD
        COMMAND "${CMAKE_COMMAND}"
            -D CONFIG=$<CONFIG>
            -D TARGET_PDB=$<TARGET_PDB_FILE:${target_name}>
            -D TARGET_DIR=$<TARGET_FILE_DIR:${target_name}>
            -D OUT_DIR=$<TARGET_FILE_DIR:${target_name}>
            -D RUNTIME_DLLS=$<TARGET_RUNTIME_DLLS:${target_name}>
            -D EXTRA_DLL_DIRS="${ARGN}"
            -P "${_script_path}"
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
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