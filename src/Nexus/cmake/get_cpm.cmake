set(CPM_DOWNLOAD_VERSION 0.42.1)

if(NOT CPM_SOURCE_CACHE)
  set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/cpm-cache")
endif()

set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
  message(STATUS "Downloading CPM.cmake")
  file(DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
    ${CPM_DOWNLOAD_LOCATION}
  )
endif()

include(${CPM_DOWNLOAD_LOCATION})
