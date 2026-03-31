vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO g-truc/glm
    REF ${VERSION}
    SHA512 0a490f0c79cd4a8ba54f37358f8917cef961dab9e61417c84ae0959c61bc860e5b83f4fb7f27169fb3d08eef1d84131bddde23d60876922310205c901b1273aa
)

# Install headers
file(INSTALL
    "${SOURCE_PATH}/glm"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
)

# Install minimal CMake config (no version file!)
file(WRITE "${CURRENT_PACKAGES_DIR}/share/glm/glmConfig.cmake" "
include(\${CMAKE_CURRENT_LIST_DIR}/glmTargets.cmake)
")

# Create a dummy target for glm::glm
file(WRITE "${CURRENT_PACKAGES_DIR}/share/glm/glmTargets.cmake" "
add_library(glm::glm INTERFACE IMPORTED)
set_target_properties(glm::glm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES \"${CURRENT_PACKAGES_DIR}/include\"
)
")

# Usage file
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
     DESTINATION "${CURRENT_PACKAGES_DIR}/share/glm")
