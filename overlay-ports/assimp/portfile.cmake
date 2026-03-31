vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO assimp/assimp
    REF v6.0.2
    SHA512 dc9637b183a1ab4c87d3548b1cacf4278fc5d30ffa4ca35436f94723c20b916932791e8e2c2f0d2a63786078457e61a42fb7aac8462551172f7f5bd2582ad9a9
)

# Assimp tries to build tools, tests, samples, exporters, etc.
# We disable everything except the core import library.
set(ASSIMP_OPTIONS
    -DASSIMP_BUILD_TESTS=OFF
    -DASSIMP_BUILD_SAMPLES=OFF
    -DASSIMP_BUILD_DOCS=OFF
    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
    -DASSIMP_BUILD_ZLIB=OFF
    -DASSIMP_NO_EXPORT=ON
    -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON
    -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF
    -DASSIMP_BUILD_DRACO=OFF
    -DASSIMP_BUILD_NONFREE_C4D_IMPORTER=OFF
)

# Emscripten-specific fixes
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    list(APPEND ASSIMP_OPTIONS
        -DASSIMP_BUILD_NO_OWN_ZLIB=ON
        -DASSIMP_BUILD_NO_OWN_BZIP2=ON
        -DASSIMP_BUILD_NO_OWN_MINIZIP=ON
    )
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${ASSIMP_OPTIONS}
)

vcpkg_cmake_install()

# Remove ALL version files (they reject wasm32)
file(GLOB VERSION_FILES
    "${CURRENT_PACKAGES_DIR}/share/assimp/*ConfigVersion.cmake"
    "${CURRENT_PACKAGES_DIR}/lib/cmake/assimp-*/*ConfigVersion.cmake"
    "${CURRENT_PACKAGES_DIR}/debug/lib/cmake/assimp-*/*ConfigVersion.cmake"
)

foreach(f ${VERSION_FILES})
    file(REMOVE "${f}")
endforeach()


# Ensure debug/share/assimp exists so fixup doesn't fail
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/debug/share/assimp")

# Fix up config
vcpkg_cmake_config_fixup()

# Cleanup
file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

vcpkg_copy_pdbs()

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/usage"
    "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage"
    COPYONLY
)