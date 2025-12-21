cmake_minimum_required(VERSION 3.28)

project ("Nexus-Audio")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include (FetchContent)

set(AUDIO_SOURCES 
	"include/Audio/Audio-Core.hpp"
	"include/Audio/AudioBuffer.hpp"
	"include/Audio/AudioDevice.hpp" 
	"include/Audio/AudioLoader.hpp" 
	"include/Audio/AudioSource.hpp" 
	"include/Audio/AudioTypes.hpp"
	"src/AudioLoader.cpp"
	"include/OpenAL/OpenAL-API.hpp"
	"src/OpenAL-API.cpp")

set(PUBLIC_LIBRARY_INCLUDES
	"include"
)

set(PRIVATE_LIBRARY_INCLUDES "")

set(LIBRARY_COMPILE_DEFINITIONS "NX_AUDIO_EXPORT=1")

FetchContent_Declare(
	libnyquist
	GIT_REPOSITORY https://github.com/ddiakopoulos/libnyquist.git
	GIT_TAG 767efd9
)
FetchContent_MakeAvailable(libnyquist)

list (APPEND NEXUS_LINKED_LIBRARIES libnyquist)

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
	  list (APPEND NEXUS_LINKED_LIBARIES ${OPENAL_LIBRARY})
	else()
		FetchContent_Declare(
			openal-soft
			GIT_REPOSITORY https://github.com/kcat/openal-soft.git
			GIT_TAG bb2c413
		)
		FetchContent_MakeAvailable(openal-soft)

	  list (APPEND NEXUS_LINKED_LIBRARIES OpenAL::OpenAL)
	endif()
endif()

add_library(Nexus-Audio ${AUDIO_SOURCES})

target_compile_definitions(Nexus-Audio PUBLIC ${LIBRARY_COMPILE_DEFINITIONS})
target_include_directories(Nexus-Audio PUBLIC ${PUBLIC_LIBRARY_INCLUDES})
target_include_directories(Nexus-Audio PRIVATE ${PRIVATE_LIBRARY_INCLUDES})
target_link_libraries(Nexus-Audio PUBLIC ${NEXUS_LINKED_LIBRARIES})