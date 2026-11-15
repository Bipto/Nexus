add_library(Nexus-Maths INTERFACE)

target_include_directories(Nexus-Maths INTERFACE include)

target_compile_features(Nexus-Maths INTERFACE cxx_std_23)

if (BUILD_TESTING)
	add_subdirectory(tests)
endif()