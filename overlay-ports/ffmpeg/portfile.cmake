vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ffmpeg/ffmpeg
    REF n6.1.1
    SHA512 a84209fe36a2a0262ebc34b727e7600b12d4739991a95599d7b4df533791b12e2e43586ccc6ff26aab2f935a3049866204e322ec0c5e49e378fc175ded34e183
)

# Path to Git Bash
set(BASH "C:/Program Files/Git/usr/bin/bash.exe")

# Emscripten environment
set(ENV{EMSDK} "C:/emsdk")
set(ENV{EMSDK_NODE} "C:/emsdk/node/20.18.0_64bit/bin/node.exe")
set(ENV{EMSDK_PYTHON} "C:/emsdk/python/3.9.2-nuget_64bit/python.exe")
set(ENV{PATH} "C:/emsdk/upstream/emscripten;C:/emsdk/node/20.18.0_64bit/bin;$ENV{PATH}")

# Convert vcpkg prefix to POSIX path
file(TO_CMAKE_PATH "${CURRENT_PACKAGES_DIR}" PREFIX_POSIX)
string(REPLACE "C:" "/c" PREFIX_POSIX "${PREFIX_POSIX}")

# Wrapper script for FFmpeg configure
file(WRITE ${SOURCE_PATH}/emscripten_configure.sh
"#!/usr/bin/env bash
set -e

PREFIX=\"$1\"

./configure \
  --prefix=\"$PREFIX\" \
  --target-os=none \
  --arch=wasm32 \
  --cpu=generic \
  --enable-cross-compile \
  --cc=emcc \
  --cxx=em++ \
  --ar=emar \
  --ranlib=emranlib \
  --disable-asm \
  --disable-pthreads \
  --disable-network \
  --disable-programs \
  --disable-doc \
  --disable-debug \
  --disable-everything \
  --enable-avutil \
  --enable-avcodec \
  --enable-avformat \
  --enable-swresample \
  --enable-swscale
")

# Make script executable
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "chmod +x emscripten_configure.sh"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Configure
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "./emscripten_configure.sh ${PREFIX_POSIX}"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Build
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "emmake /c/msys64/usr/bin/make -j$(nproc)"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Install
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "emmake /c/msys64/usr/bin/make install"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Flatten FFmpeg's nested include directories
file(GLOB_RECURSE FF_HEADERS "${CURRENT_PACKAGES_DIR}/include/*")
foreach(h ${FF_HEADERS})
    get_filename_component(dir "${h}" DIRECTORY)
    if(dir MATCHES "libav")
        file(COPY "${h}" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
    endif()
endforeach()

# Remove nested include dirs
file(GLOB SUBDIRS "${CURRENT_PACKAGES_DIR}/include/*")
foreach(d ${SUBDIRS})
    if(IS_DIRECTORY "${d}")
        file(REMOVE_RECURSE "${d}")
    endif()
endforeach()

# Fix pkg-config files
vcpkg_fixup_pkgconfig()

# Generate CMake config
set(CONFIG_DIR "${CURRENT_PACKAGES_DIR}/share/ffmpeg")
file(MAKE_DIRECTORY "${CONFIG_DIR}")

file(WRITE "${CONFIG_DIR}/FFmpegConfig.cmake" "
include(CMakeFindDependencyMacro)

set(_ffmpeg_libs avutil avcodec avformat swresample swscale)

foreach(lib IN LISTS _ffmpeg_libs)
    add_library(FFmpeg::\${lib} UNKNOWN IMPORTED)

    set_target_properties(FFmpeg::\${lib} PROPERTIES
        IMPORTED_LOCATION \"\${CMAKE_CURRENT_LIST_DIR}/../../lib/lib\${lib}.a\"
        INTERFACE_INCLUDE_DIRECTORIES \"\${CMAKE_CURRENT_LIST_DIR}/../../include\"
    )
endforeach()
")

file(WRITE "${CONFIG_DIR}/FFmpegConfigVersion.cmake" "
set(PACKAGE_VERSION \"6.1.1\")

if(PACKAGE_FIND_VERSION VERSION_EQUAL PACKAGE_VERSION)
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
    set(PACKAGE_VERSION_EXACT TRUE)
elseif(PACKAGE_FIND_VERSION VERSION_LESS PACKAGE_VERSION)
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
else()
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
endif()
")

file(WRITE "${CONFIG_DIR}/usage" "
FFmpeg was built for Emscripten.

Use it in CMake with:

    find_package(FFmpeg CONFIG REQUIRED)
    target_link_libraries(main PRIVATE FFmpeg::avcodec FFmpeg::avformat FFmpeg::avutil FFmpeg::swscale FFmpeg::swresample)
")
