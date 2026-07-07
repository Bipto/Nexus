include(FetchContent)

set(LOGGING_SOURCES
	include/Logging/Logger.hpp
	src/Logging/Logger.cpp
)

add_library(Nexus-Logging INTERFACE ${LOGGING_SOURCES})
target_include_directories(Nexus-Logging INTERFACE include)

target_compile_features(Nexus-Logging INTERFACE cxx_std_23)
set_target_properties(Nexus-Logging PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

if (BUILD_TESTING)
	add_subdirectory(tests)
endif()