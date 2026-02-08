cmake_minimum_required(VERSION 3.30)

project ("Nexus-Audio")

include (FetchContent)

set(AUDIO_SOURCES 
	"include/Audio/AudioBuffer.hpp"
	"include/Audio/Audio-Core.hpp"
	"include/Audio/AudioDevice.hpp" 
	"include/Audio/AudioLoader.hpp" 
	"include/Audio/AudioSource.hpp" 
	"include/Audio/AudioTypes.hpp"
	"include/OpenAL/OpenAL-API.hpp"
	"src/AudioLoader.cpp"
	"src/OpenAL-API.cpp")

set(PUBLIC_LIBRARY_INCLUDES
	"include"
)

set(PRIVATE_LIBRARY_INCLUDES "")

set(LIBRARY_COMPILE_DEFINITIONS "NX_AUDIO_EXPORT=1")

if (DEFINED NX_PLATFORM_OPENAL)
	list (APPEND LIBRARY_COMPILE_DEFINITIONS "NX_PLATFORM_OPENAL=1")

	list (APPEND AUDIO_SOURCES
		"src/OpenAL/AudioBufferOpenAL.cpp"
		"src/OpenAL/AudioBufferOpenAL.hpp"
		"src/OpenAL/AudioDeviceOpenAL.cpp"
		"src/OpenAL/AudioDeviceOpenAL.hpp"
		"src/OpenAL/AudioSourceOpenAL.cpp"
		"src/OpenAL/AudioSourceOpenAL.hpp"
		"src/OpenAL/OpenAL.cpp"
		"src/OpenAL/OpenAL.hpp"
	)

	list (APPEND PRIVATE_LIBRARY_INCLUDES
		"${CMAKE_CURRENT_SOURCE_DIR}/src/OpenAL"
	)

	if (EMSCRIPTEN)
	  find_package(OpenAL REQUIRED)
	  list (APPEND NEXUS_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
	  list (APPEND NEXUS_LINKED_LIBRARIES ${OPENAL_LIBRARY})
	else()
	  FetchContent_Declare(
		openal-soft
		GIT_REPOSITORY https://github.com/kcat/openal-soft.git
		GIT_TAG 1.25.1
	  )
	  FetchContent_MakeAvailable(openal-soft)
	  list (APPEND NEXUS_LINKED_LIBRARIES OpenAL::OpenAL)
	endif()
endif()

add_library(Nexus-Audio ${AUDIO_SOURCES})

target_compile_features(Nexus-Audio PUBLIC cxx_std_20)
set_target_properties(Nexus-Audio PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

FetchContent_Declare(
	tl_expected
	GIT_REPOSITORY https://github.com/TartanLlama/expected.git
	GIT_TAG v1.3.1
)
FetchContent_MakeAvailable(tl_expected)
target_link_libraries(Nexus-Audio PUBLIC tl::expected)

target_compile_definitions(Nexus-Audio PUBLIC ${LIBRARY_COMPILE_DEFINITIONS})
target_include_directories(Nexus-Audio PUBLIC ${PUBLIC_LIBRARY_INCLUDES})
target_include_directories(Nexus-Audio PRIVATE ${PRIVATE_LIBRARY_INCLUDES})
target_link_libraries(Nexus-Audio PRIVATE ${NEXUS_LINKED_LIBRARIES})

#ffmpeg
if (MINGW)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
		libavcodec libavformat libavutil libswscale libswresample
	)

	target_link_libraries(Nexus-Audio PRIVATE PkgConfig::FFMPEG)
else()
	find_package(FFMPEG REQUIRED)
	target_include_directories(Nexus-Audio PRIVATE ${FFMPEG_INCLUDE_DIRS})
	target_link_directories(Nexus-Audio PRIVATE ${FFMPEG_LIBRARY_DIRS})
	target_link_libraries(Nexus-Audio PRIVATE ${FFMPEG_LIBRARIES})
endif()