# ------------------------------------------------------------
# Subdirectories
# ------------------------------------------------------------
add_subdirectory(Nexus)

if (NX_BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

if (NX_BUILD_DEMO)
    add_subdirectory(Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()