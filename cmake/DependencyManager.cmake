include(FetchContent)
include(CMakeParseArguments)

function(GenerateConfigFile NAME INSTALL_DIR LIB_FILENAME)
    set(CONFIG_DIR "${INSTALL_DIR}/lib/cmake/${NAME}")
    file(MAKE_DIRECTORY "${CONFIG_DIR}")

    set(CONFIG_FILE "${CONFIG_DIR}/${NAME}Config.cmake")
    file(WRITE "${CONFIG_FILE}" "
# Auto-generated config for ${NAME}
add_library(${NAME}::${NAME} UNKNOWN IMPORTED)
set_target_properties(${NAME}::${NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES \"${INSTALL_DIR}/include\"
    IMPORTED_LOCATION \"${INSTALL_DIR}/lib/${LIB_FILENAME}\"
)
")
endfunction()

function(FetchContentWithCache NAME)
    cmake_policy(PUSH)
    if(POLICY CMP0169)
        cmake_policy(SET CMP0169 OLD)
    endif()

    # --- Parse arguments ---
    set(options FORCE_REBUILD)
    set(oneValueArgs URL GIT_TAG)
    set(multiValueArgs CONFIG_OPTIONS)
    cmake_parse_arguments(FETCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT FETCH_URL)
        message(FATAL_ERROR "FetchContentWithCache requires URL for ${NAME}")
    endif()

    # --- Compute unique hash ---
    string(CONCAT HASH_INPUT "${FETCH_URL}_${FETCH_GIT_TAG}")
    foreach(opt ${FETCH_CONFIG_OPTIONS})
        string(APPEND HASH_INPUT "_${opt}")
    endforeach()
    string(APPEND HASH_INPUT "_${CMAKE_C_COMPILER_ID}_${CMAKE_C_COMPILER_VERSION}")
    string(APPEND HASH_INPUT "_${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_VERSION}")
    if(DEFINED CMAKE_SYSTEM_PROCESSOR)
        string(APPEND HASH_INPUT "_${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    string(SHA256 FETCH_HASH "${HASH_INPUT}")

    # --- Directories ---
    set(BASE_DIR "${CMAKE_SOURCE_DIR}/cache/${NAME}/${FETCH_HASH}")
    set(SRC_DIR "${BASE_DIR}/src")
    set(BUILD_DIR "${BASE_DIR}/build")
    set(INSTALL_DIR "${BASE_DIR}/install")

    message(STATUS "Fetching ${NAME} -> ${SRC_DIR}")
    message(STATUS "Cache hash: ${FETCH_HASH}")
    message(STATUS "Install path: ${INSTALL_DIR}")

    set(NEED_BUILD TRUE)

    # --- Check cache ---
    if(NOT FETCH_FORCE_REBUILD AND EXISTS "${INSTALL_DIR}/lib/cmake")
        message(STATUS "Using cached install of ${NAME} at ${INSTALL_DIR}")
        set(NEED_BUILD FALSE)
    endif()

    if(NEED_BUILD)
        # --- Fetch source ---
        FetchContent_Declare(${NAME}
            GIT_REPOSITORY ${FETCH_URL}
            GIT_TAG ${FETCH_GIT_TAG}
            GIT_SHALLOW TRUE
        )
        FetchContent_Populate(${NAME})

        # --- Build & install ---
        file(MAKE_DIRECTORY "${BUILD_DIR}")
        file(MAKE_DIRECTORY "${INSTALL_DIR}")

        if(DEFINED CMAKE_CONFIGURATION_TYPES)
            set(CONFIGS ${CMAKE_CONFIGURATION_TYPES})
        else()
            set(CONFIGS ${CMAKE_BUILD_TYPE})
        endif()

        foreach(CONFIG ${CONFIGS})
            message(STATUS "Configuring ${NAME} for ${CONFIG}...")
            execute_process(
                COMMAND ${CMAKE_COMMAND}
                    -S ${${NAME}_SOURCE_DIR}
                    -B ${BUILD_DIR}
                    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                    -DCMAKE_BUILD_TYPE=${CONFIG}
                    ${FETCH_CONFIG_OPTIONS}
                RESULT_VARIABLE res
            )
            if(NOT res EQUAL 0)
                message(FATAL_ERROR "CMake configure failed for ${NAME} (${CONFIG})")
            endif()

            message(STATUS "Building and installing ${NAME} for ${CONFIG}...")
            execute_process(
                COMMAND ${CMAKE_COMMAND}
                    --build ${BUILD_DIR}
                    --target install
                    --config ${CONFIG}
                RESULT_VARIABLE res2
            )
            if(NOT res2 EQUAL 0)
                message(FATAL_ERROR "Build/install failed for ${NAME} (${CONFIG})")
            endif()
        endforeach()

        file(REMOVE_RECURSE "${BUILD_DIR}")

        # --- Fallback config ---
        set(CONFIG_DIR "${INSTALL_DIR}/lib/cmake/${NAME}")
        set(CONFIG_FILE "${CONFIG_DIR}/${NAME}Config.cmake")

        if(NOT EXISTS "${CONFIG_FILE}")
            set(LIB_FILENAME "${NAME}.lib")
            if(UNIX AND NOT APPLE)
                set(LIB_FILENAME "lib${NAME}.a")
            elseif(APPLE)
                set(LIB_FILENAME "lib${NAME}.dylib")
            endif()

            message(STATUS "Generating fallback Config.cmake for ${NAME}")
            GenerateConfigFile(${NAME} "${INSTALL_DIR}" "${LIB_FILENAME}")
        endif()
    endif()

    # --- Always add prefix path ---
    list(APPEND CMAKE_PREFIX_PATH "${INSTALL_DIR}")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE STRING "" FORCE)

    cmake_policy(POP)
endfunction()
