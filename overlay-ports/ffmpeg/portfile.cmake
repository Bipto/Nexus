vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ffmpeg/ffmpeg
    REF n6.1.1
    SHA512 a84209fe36a2a0262ebc34b727e7600b12d4739991a95599d7b4df533791b12e2e43586ccc6ff26aab2f935a3049866204e322ec0c5e49e378fc175ded34e183
)

# Path to Git Bash (needed to run FFmpeg's POSIX configure script)
set(BASH "C:/Program Files/Git/usr/bin/bash.exe")

# Ensure Emscripten tools are visible inside vcpkg's build environment
set(ENV{EMSDK} "C:/emsdk")
set(ENV{EMSDK_NODE} "C:/emsdk/node/20.18.0_64bit/bin/node.exe")
set(ENV{EMSDK_PYTHON} "C:/emsdk/python/3.9.2-nuget_64bit/python.exe")
set(ENV{PATH} "C:/emsdk/upstream/emscripten;C:/emsdk/node/20.18.0_64bit/bin;$ENV{PATH}")

# Convert vcpkg's install prefix to a POSIX-style path for Bash
file(TO_CMAKE_PATH "${CURRENT_PACKAGES_DIR}" PREFIX_POSIX)
# Adjust drive letter mapping if needed (here for C:)
string(REPLACE "C:" "/c" PREFIX_POSIX "${PREFIX_POSIX}")

# Write a small wrapper script to avoid complex quoting issues
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

# Make the wrapper script executable
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "chmod +x emscripten_configure.sh"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Run FFmpeg configure via the wrapper script
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "./emscripten_configure.sh ${PREFIX_POSIX}"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Build using MSYS2 make explicitly
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "emmake /c/msys64/usr/bin/make -j$(nproc)"
    WORKING_DIRECTORY ${SOURCE_PATH}
)

# Install
vcpkg_execute_required_process(
    COMMAND ${BASH} -lc "emmake /c/msys64/usr/bin/make install"
    WORKING_DIRECTORY ${SOURCE_PATH}
)


# Fix pkg-config files (if any)
vcpkg_fixup_pkgconfig()
