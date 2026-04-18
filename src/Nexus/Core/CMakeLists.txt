include(FetchContent)

set(CORE_SOURCES
	include/Core/AutoRelease.hpp
	include/Core/AutoRelease.inl
	include/Core/MemorySize.hpp
	include/Core/NamedJThread.hpp
	include/Core/NamedJThread.inl
	include/Core/ResourcePool.hpp
	include/Core/ResourcePool.inl
	include/Core/ThreadPool.hpp
	include/Core/ThreadPool.inl
	include/Profiling/Profiler.hpp
	src/Core/MemorySize.cpp
)

add_library(Nexus-Core ${CORE_SOURCES})
target_include_directories(Nexus-Core PUBLIC include)

target_compile_features(Nexus-Core PUBLIC cxx_std_23)
set_target_properties(Nexus-Core PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

if (BUILD_TESTING)
	add_subdirectory(tests)
endif()

##################################################################################
#-----------------------------------Tracy-----------------------------------------
##################################################################################
# Skip Tracy when using MinGW or Clang-cl
if (NOT MINGW AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))

	FetchContent_Declare(
		tracy
		GIT_REPOSITORY https://github.com/wolfpld/tracy.git
		GIT_TAG v0.13.1
	)
	FetchContent_MakeAvailable(tracy)

	if (EMSCRIPTEN)
		add_library(Tracy-Static
			${tracy_SOURCE_DIR}/public/TracyClient.cpp
		)

		target_include_directories(Tracy-Static PUBLIC
			${tracy_SOURCE_DIR}/public
		)

		target_compile_definitions(Tracy-Static PUBLIC TRACY_ENABLE)
		target_compile_definitions(Nexus-Core PUBLIC TRACY_IMPORTS)

		set_target_properties(Tracy-Static PROPERTIES
			CXX_VISIBILITY_PRESET hidden
			VISIBILITY_INLINES_HIDDEN YES
		)
		target_link_libraries(Nexus-Core PUBLIC Tracy-Static)
	else()
		add_library(Tracy-Shared SHARED
			${tracy_SOURCE_DIR}/public/TracyClient.cpp
		)

		target_include_directories(Tracy-Shared PUBLIC
			${tracy_SOURCE_DIR}/public
		)

		set_target_properties(Tracy-Shared PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

		target_compile_definitions(Tracy-Shared PUBLIC TRACY_ENABLE)
		target_compile_definitions(Nexus-Core PUBLIC TRACY_IMPORTS)

		set_target_properties(Tracy-Shared PROPERTIES
			CXX_VISIBILITY_PRESET hidden
			VISIBILITY_INLINES_HIDDEN YES
		)
		target_link_libraries(Nexus-Core PUBLIC Tracy-Shared)
	endif()


	add_definitions(-DTRACY_ENABLE)


	set(NX_TRACY_AVAILABLE ON CACHE BOOL "")

endif()