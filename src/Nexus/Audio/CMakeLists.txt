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

set(NEXUS_LINKED_LIBRARIES Nexus-Core)

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
		FetchContentWithCache(
			openal-soft
			URL https://github.com/kcat/openal-soft.git
			GIT_TAG 1.25.1
			CONFIG_OPTIONS "-DBUILD_TESTING=OFF")

		find_package(OpenAL CONFIG REQUIRED)

	  list (APPEND NEXUS_LINKED_LIBRARIES OpenAL::OpenAL)
	endif()
endif()

add_library(Nexus-Audio ${AUDIO_SOURCES})

target_compile_features(Nexus-Audio PUBLIC cxx_std_20)
set_target_properties(Nexus-Audio PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

FetchContentWithCache(
	sndfile
	URL https://github.com/libsndfile/libsndfile.git
	GIT_TAG 68f6c16
	CONFIG_OPTIONS 
		-DBUILD_TESTING=OFF
		-DDENABLE_CMAKE_CONFIG=ON
)

find_package(sndfile CONFIG REQUIRED)
target_link_libraries(Nexus-Audio PRIVATE SndFile::sndfile)

target_compile_definitions(Nexus-Audio PUBLIC ${LIBRARY_COMPILE_DEFINITIONS})
target_include_directories(Nexus-Audio PUBLIC ${PUBLIC_LIBRARY_INCLUDES})
target_include_directories(Nexus-Audio PRIVATE ${PRIVATE_LIBRARY_INCLUDES})
target_link_libraries(Nexus-Audio PRIVATE ${NEXUS_LINKED_LIBRARIES})