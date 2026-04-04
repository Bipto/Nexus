include(FetchContent)
include(CMakeParseArguments)
include(ProcessorCount)

# -------------------------------------------------------------
# Global parallelism detection
# -------------------------------------------------------------
ProcessorCount(NPROC)
if(NOT NPROC OR NPROC EQUAL 0)
    set(NPROC 4)
endif()

# -------------------------------------------------------------
# Generate fallback Config.cmake when a project does not install one
# -------------------------------------------------------------
function(GenerateConfigFile NAME INSTALL_DIR)
    set(CONFIG_DIR "${INSTALL_DIR}/lib/cmake/${NAME}")
    file(MAKE_DIRECTORY "${CONFIG_DIR}")

    # --- Search for all possible library types ---
    file(GLOB LIB_SHARED
        "${INSTALL_DIR}/bin/${NAME}.dll"
        "${INSTALL_DIR}/lib/${NAME}.dll"
        "${INSTALL_DIR}/lib/lib${NAME}.so"
        "${INSTALL_DIR}/lib/lib${NAME}.dylib"
    )

    file(GLOB LIB_STATIC
        "${INSTALL_DIR}/lib/${NAME}.lib"        # static or import
        "${INSTALL_DIR}/lib/lib${NAME}.a"
    )

    set(IMPORTED_LOCATION "")
    set(IMPORTED_IMPLIB "")

    # --- Prefer shared libraries ---
    if(LIB_SHARED)
        list(GET LIB_SHARED 0 SHARED_LIB)

        if(SHARED_LIB MATCHES "\\.dll$")
            # DLL case: need both DLL + import lib
            set(IMPORTED_LOCATION "${SHARED_LIB}")

            # Find import library
            file(GLOB DLL_IMPLIB
                "${INSTALL_DIR}/lib/${NAME}.lib"
                "${INSTALL_DIR}/lib/lib${NAME}.a"
            )
            if(DLL_IMPLIB)
                list(GET DLL_IMPLIB 0 IMPORTED_IMPLIB)
            endif()
        else()
            # Unix shared library
            set(IMPORTED_LOCATION "${SHARED_LIB}")
        endif()

    elseif(LIB_STATIC)
        # Static library
        list(GET LIB_STATIC 0 STATIC_LIB)
        set(IMPORTED_LOCATION "${STATIC_LIB}")

    else()
        message(WARNING "Could not detect library for ${NAME}, defaulting to lib${NAME}.a")
        set(IMPORTED_LOCATION "${INSTALL_DIR}/lib/lib${NAME}.a")
    endif()

    # --- Write config file ---
    set(CONFIG_FILE "${CONFIG_DIR}/${NAME}Config.cmake")
    file(WRITE "${CONFIG_FILE}" "
# Auto-generated config for ${NAME}
add_library(${NAME}::${NAME} UNKNOWN IMPORTED)
set_target_properties(${NAME}::${NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES \"${INSTALL_DIR}/include\"
    IMPORTED_LOCATION \"${IMPORTED_LOCATION}\"
")

    if(IMPORTED_IMPLIB)
        file(APPEND "${CONFIG_FILE}" "
    IMPORTED_IMPLIB \"${IMPORTED_IMPLIB}\"
")
    endif()

    file(APPEND "${CONFIG_FILE}" "
)
")
endfunction()

# -------------------------------------------------------------
# FetchContentWithCache
# -------------------------------------------------------------
function(FetchContentWithCache NAME)
    cmake_policy(PUSH)
    if(POLICY CMP0169)
        cmake_policy(SET CMP0169 OLD)
    endif()

    set(options FORCE_REBUILD)
    set(oneValueArgs URL GIT_TAG)
    set(multiValueArgs CONFIG_OPTIONS)
    cmake_parse_arguments(FETCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT FETCH_URL)
        message(FATAL_ERROR "FetchContentWithCache requires URL for ${NAME}")
    endif()

    # --- Compute unique hash (URL + tag + config options + arch) ---
    string(CONCAT HASH_INPUT "${FETCH_URL}_${FETCH_GIT_TAG}")
    foreach(opt ${FETCH_CONFIG_OPTIONS})
        string(APPEND HASH_INPUT "_${opt}")
    endforeach()
    if(DEFINED CMAKE_SYSTEM_PROCESSOR)
        string(APPEND HASH_INPUT "_${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    string(SHA256 FETCH_HASH "${HASH_INPUT}")

    # --- Directories ---
    set(BASE_DIR "${CMAKE_SOURCE_DIR}/cache/${NAME}/${FETCH_HASH}")
    set(BUILD_DIR "${BASE_DIR}/build")
    set(INSTALL_DIR "${BASE_DIR}/install")

    message(STATUS "Fetching ${NAME}")
    message(STATUS "Cache hash: ${FETCH_HASH}")
    message(STATUS "Install path: ${INSTALL_DIR}")

    set(NEED_BUILD TRUE)

    if(NOT FETCH_FORCE_REBUILD AND EXISTS "${INSTALL_DIR}/lib/cmake")
        message(STATUS "Using cached install of ${NAME}")
        set(NEED_BUILD FALSE)
    endif()

    if(NEED_BUILD)
        # --- Fetch source ---
        if(FETCH_URL MATCHES "\\.git$")
            FetchContent_Declare(${NAME}
                GIT_REPOSITORY ${FETCH_URL}
                GIT_TAG ${FETCH_GIT_TAG}
                GIT_SHALLOW TRUE
            )
        else()
            FetchContent_Declare(${NAME}
                URL ${FETCH_URL}
            )
        endif()

        FetchContent_Populate(${NAME})

        # ---------------------------------------------------------
        # Robust source directory detection (SDL3-safe)
        # ---------------------------------------------------------
        set(SRC_DIR_CANDIDATES "")

        # 1. Exact match
        if(DEFINED ${NAME}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME}_SOURCE_DIR}")
        endif()

        # 2. Strip trailing digits (SDL3 → SDL)
        string(REGEX REPLACE "[0-9]+$" "" NAME_STRIPPED "${NAME}")
        if(DEFINED ${NAME_STRIPPED}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_STRIPPED}_SOURCE_DIR}")
        endif()

        # 3. Lowercase variants
        string(TOLOWER "${NAME}" NAME_LC)
        if(DEFINED ${NAME_LC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_LC}_SOURCE_DIR}")
        endif()

        string(TOLOWER "${NAME_STRIPPED}" NAME_STRIPPED_LC)
        if(DEFINED ${NAME_STRIPPED_LC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_STRIPPED_LC}_SOURCE_DIR}")
        endif()

        # 4. Uppercase variants
        string(TOUPPER "${NAME}" NAME_UC)
        if(DEFINED ${NAME_UC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_UC}_SOURCE_DIR}")
        endif()

        string(TOUPPER "${NAME_STRIPPED}" NAME_STRIPPED_UC)
        if(DEFINED ${NAME_STRIPPED_UC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_STRIPPED_UC}_SOURCE_DIR}")
        endif()

        # 5. Pick the first existing directory
        set(SRC_DIR "")
        foreach(candidate ${SRC_DIR_CANDIDATES})
            if(EXISTS "${candidate}/CMakeLists.txt")
                set(SRC_DIR "${candidate}")
                break()
            endif()
        endforeach()

        if(NOT SRC_DIR)
            message(FATAL_ERROR "FetchContent did not produce a valid source directory for ${NAME}. Checked: ${SRC_DIR_CANDIDATES}")
        endif()

        # --- Configure ---
        file(MAKE_DIRECTORY "${BUILD_DIR}")
        file(MAKE_DIRECTORY "${INSTALL_DIR}")

        execute_process(
            COMMAND ${CMAKE_COMMAND}
                -S ${SRC_DIR}
                -B ${BUILD_DIR}
                -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                ${FETCH_CONFIG_OPTIONS}
            RESULT_VARIABLE res
        )
        if(NOT res EQUAL 0)
            message(FATAL_ERROR "CMake configure failed for ${NAME}")
        endif()

        # --- Build & install (parallel) ---
        if(CMAKE_GENERATOR MATCHES "Visual Studio|Xcode|Multi-Config")
            foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
                execute_process(
                    COMMAND ${CMAKE_COMMAND}
                        --build ${BUILD_DIR}
                        --config ${CONFIG}
                        --target install
                        --parallel ${NPROC}
                    RESULT_VARIABLE res2
                )
                if(NOT res2 EQUAL 0)
                    message(FATAL_ERROR "Build/install failed for ${NAME} (${CONFIG})")
                endif()
            endforeach()
        else()
            execute_process(
                COMMAND ${CMAKE_COMMAND}
                    --build ${BUILD_DIR}
                    --target install
                    --parallel ${NPROC}
                RESULT_VARIABLE res3
            )
            if(NOT res3 EQUAL 0)
                message(FATAL_ERROR "Build/install failed for ${NAME}")
            endif()
        endif()

        # --- Fallback config if package didn't install one ---
        set(CONFIG_FILE "${INSTALL_DIR}/lib/cmake/${NAME}/${NAME}Config.cmake")
        if(NOT EXISTS "${CONFIG_FILE}")
            message(STATUS "Generating fallback Config.cmake for ${NAME}")
            GenerateConfigFile(${NAME} "${INSTALL_DIR}")
        endif()
    endif()

    # Expose hash
    set(${NAME}_FETCH_HASH "${FETCH_HASH}" PARENT_SCOPE)

    # Append to prefix path
    list(APPEND CMAKE_PREFIX_PATH "${INSTALL_DIR}")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE STRING "Prefix paths" FORCE)

    cmake_policy(POP)
endfunction()
