include(FetchContent)
include(CMakeParseArguments)

function(FetchContentWithCache NAME)
    set(options "")
    set(oneValueArgs URL GIT_TAG)
    set(multiValueArgs CONFIG_OPTIONS)
    cmake_parse_arguments(FETCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT FETCH_URL)
        message(FATAL_ERROR "FetchContentWithCache requires URL")
    endif()

    # Compute a hash based on URL + GIT_TAG + options
    string(CONCAT FETCH_ID "${FETCH_URL}" "_" "${FETCH_GIT_TAG}")
    foreach(opt ${FETCH_CONFIG_OPTIONS})
        string(APPEND FETCH_ID "_" "${opt}")
    endforeach()
    
    # Hash it
    string(SHA256 FETCH_HASH "${FETCH_ID}")

    # Define a per-dependency binary dir
    set(FETCH_BIN_DIR "${CMAKE_SOURCE_DIR}/cache/${NAME}/${FETCH_HASH}")

    message(STATUS "Fetching ${NAME} -> ${FETCH_BIN_DIR}")

    # Configure FetchContent to use that build dir
    set(FETCHCONTENT_BASE_DIR "${FETCH_BIN_DIR}")
    set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

    FetchContent_Declare(
        ${NAME}
        GIT_REPOSITORY ${FETCH_URL}
        GIT_TAG ${FETCH_GIT_TAG}
    )

    # Apply custom CMake options
    set(BUILD_DIR "${FETCH_BIN_DIR}/build")
    set(FETCHCONTENT_SOURCE_DIR "${FETCH_BIN_DIR}/src")
    set(FETCHCONTENT_BINARY_DIR "${BUILD_DIR}")

    # Make available
    FetchContent_MakeAvailable(${NAME})
endfunction()