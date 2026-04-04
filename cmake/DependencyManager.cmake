include(FetchContent)
include(CMakeParseArguments)
include(ProcessorCount)

# -------------------------------------------------------------
# Global parallelism detection
# -------------------------------------------------------------
ProcessorCount(NPROC)
if(NPROC EQUAL 0)
    set(NPROC 4)
endif()

# -------------------------------------------------------------
# Compatibility table: required flags for known libraries
# -------------------------------------------------------------
function(_nexus_get_required_flags NAME OUT_VAR)
    set(FLAGS "")

    # GoogleTest
    if(NAME STREQUAL "googletest" OR NAME STREQUAL "gtest")
        list(APPEND FLAGS
            -DINSTALL_GTEST=ON
            -DINSTALL_GMOCK=ON
            -DBUILD_GMOCK=ON
        )

    # Assimp
    elseif(NAME STREQUAL "assimp")
        list(APPEND FLAGS
            -DASSIMP_INSTALL=ON
            -DASSIMP_BUILD_TESTS=OFF
        )

    # SDL3
    elseif(NAME STREQUAL "SDL3" OR NAME STREQUAL "SDL")
        list(APPEND FLAGS
            -DSDL_INSTALL=ON
        )

    # Freetype
    elseif(NAME STREQUAL "freetype")
        list(APPEND FLAGS
            -DFT_DISABLE_ZLIB=OFF
            -DFT_DISABLE_BZIP2=OFF
        )
    endif()

    set(${OUT_VAR} "${FLAGS}" PARENT_SCOPE)
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

        if(DEFINED ${NAME}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME}_SOURCE_DIR}")
        endif()

        string(REGEX REPLACE "[0-9]+$" "" NAME_STRIPPED "${NAME}")
        if(DEFINED ${NAME_STRIPPED}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_STRIPPED}_SOURCE_DIR}")
        endif()

        string(TOLOWER "${NAME}" NAME_LC)
        if(DEFINED ${NAME_LC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_LC}_SOURCE_DIR}")
        endif()

        string(TOLOWER "${NAME_STRIPPED}" NAME_STRIPPED_LC)
        if(DEFINED ${NAME_STRIPPED_LC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_STRIPPED_LC}_SOURCE_DIR}")
        endif()

        string(TOUPPER "${NAME}" NAME_UC)
        if(DEFINED ${NAME_UC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_UC}_SOURCE_DIR}")
        endif()

        string(TOUPPER "${NAME_STRIPPED}" NAME_STRIPPED_UC)
        if(DEFINED ${NAME_STRIPPED_UC}_SOURCE_DIR)
            list(APPEND SRC_DIR_CANDIDATES "${${NAME_STRIPPED_UC}_SOURCE_DIR}")
        endif()

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

        # --- Inject required flags for known libraries ---
        _nexus_get_required_flags(${NAME} REQUIRED_FLAGS)
        set(EFFECTIVE_CONFIG_OPTIONS ${FETCH_CONFIG_OPTIONS} ${REQUIRED_FLAGS})

        # --- Configure ---
        file(MAKE_DIRECTORY "${BUILD_DIR}")
        file(MAKE_DIRECTORY "${INSTALL_DIR}")

        execute_process(
            COMMAND ${CMAKE_COMMAND}
                -S ${SRC_DIR}
                -B ${BUILD_DIR}
                -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                ${EFFECTIVE_CONFIG_OPTIONS}
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

        # --- Require a proper CMake package config ---
        set(CONFIG_DIR "${INSTALL_DIR}/lib/cmake/${NAME}")
        if(NOT EXISTS "${CONFIG_DIR}")
            message(FATAL_ERROR "${NAME} did not install a CMake package config. Fix its build options.")
        endif()
    endif()

    # Expose hash
    set(${NAME}_FETCH_HASH "${FETCH_HASH}" PARENT_SCOPE)

    # Append to prefix path
    list(APPEND CMAKE_PREFIX_PATH "${INSTALL_DIR}")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE STRING "Prefix paths" FORCE)

    cmake_policy(POP)
endfunction()
