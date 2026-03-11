set(CORE_SOURCES
	include/Core/NamedJThread.hpp
	include/Core/ThreadPool.hpp
	)

add_library(Nexus-Core INTERFACE ${CORE_SOURCES})
target_include_directories(Nexus-Core INTERFACE include)

target_compile_features(Nexus-Core INTERFACE cxx_std_23)
set_target_properties(Nexus-Core PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

if (DEFINED BUILD_TESTING)
	add_subdirectory(tests)
endif()