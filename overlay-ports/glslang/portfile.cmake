vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/glslang
    REF 16.1.0
    SHA512 bcd0604f0a4a1a17ae207b90daeb9031d5c473968d331baf487acbc0f38871a0a82d2b20d274389f9988735e8dcd3fe4d2c2bd1513c77d031c8253c66424dbc4
)

set(GLSLANG_OPTIONS
    -DENABLE_GLSLANG_BINARIES=OFF
    -DENABLE_SPVREMAPPER=OFF
    -DENABLE_HLSL=ON
    -DENABLE_OPT=OFF
    -DBUILD_TESTING=OFF
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${GLSLANG_OPTIONS}
)

vcpkg_cmake_install()

# --- FIX: Create missing directories BEFORE fixup ---
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/share/glslang")
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/debug/share/glslang")

# Now fix up the config files
vcpkg_cmake_config_fixup()

# Remove version files
file(GLOB VERSION_FILES
    "${CURRENT_PACKAGES_DIR}/share/glslang/*ConfigVersion.cmake"
    "${CURRENT_PACKAGES_DIR}/share/glslang/*-config-version.cmake"
)
foreach(f ${VERSION_FILES})
    file(REMOVE "${f}")
endforeach()

# Overwrite configs AFTER fixup
file(WRITE "${CURRENT_PACKAGES_DIR}/share/glslang/glslang-config.cmake" "
include(\"\${CMAKE_CURRENT_LIST_DIR}/glslang-targets.cmake\")
")

file(WRITE "${CURRENT_PACKAGES_DIR}/debug/share/glslang/glslang-config.cmake" "
include(\"\${CMAKE_CURRENT_LIST_DIR}/glslang-targets.cmake\")
")

vcpkg_copy_pdbs()
