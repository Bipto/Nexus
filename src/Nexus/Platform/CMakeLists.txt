include (FetchContent)

set(PLATFORM_SOURCES 
	"include/Platform/Events/Event.hpp"
	"include/Platform/Events/EventDispatcher.hpp"
	"include/Platform/Events/EventHandler.hpp"
	"include/Platform/FileSystem/Directory.hpp"
	"include/Platform/FileSystem/File.hpp"
	"include/Platform/FileSystem/FileDialogs.hpp"
	"include/Platform/FileSystem/FileSystem.hpp"
	"include/Platform/FileSystem/Path.hpp"
	"include/Platform/Input/Events.hpp"
	"include/Platform/Input/Gamepad.hpp"
	"include/Platform/Input/Input.hpp"
	"include/Platform/Input/Keyboard.hpp"
	"include/Platform/Input/Mouse.hpp"
	"include/Platform/Logging/Log.hpp"
	"include/Platform/Timings/Clock.hpp"
	"include/Platform/Timings/DateTime.hpp"
	"include/Platform/Timings/Profiler.hpp"
	"include/Platform/Timings/Timer.hpp"
	"include/Platform/Timings/Timespan.hpp"
	"include/Platform/Unix/UnixInclude.hpp"
	"include/Platform/Utils/FramerateMonitor.hpp"
	"include/Platform/Utils/SharedLibrary.hpp"
	"include/Platform/Windows/WindowsInclude.hpp"
	"include/Platform/X11/X11Include.hpp"
	"include/Platform/IWindow.hpp"
	"include/Platform/MessageBox.hpp"
	"include/Platform/Monitor.hpp"
	"include/Platform/Platform.hpp"
	"include/Platform/WindowInfo.hpp"
	"src/Platform/FileSystem/Directory.cpp"
	"src/Platform/FileSystem/File.cpp"
	"src/Platform/FileSystem/FileSystem.cpp"
	"src/Platform/FileSystem/Path.cpp"
	"src/Platform/Input/Input.cpp"
	"src/Platform/Logging/Log.cpp"
	"src/Platform/Timings/DateTime.cpp"
	"src/Platform/Timings/Profiler.cpp"
	"src/SDL3/FileDialogsSDL3.cpp"
	"src/SDL3/FileDialogsSDL3.hpp"
	"src/SDL3/SDL3FileStream.cpp"
	"src/SDL3/SDL3FileStream.hpp"
	"src/SDL3/SDL3Include.cpp"
	"src/SDL3/SDL3Include.hpp"
	"src/SDL3/SDL3MessageBox.cpp"
	"src/SDL3/SDL3MessageBox.hpp"
	"src/SDL3/SDL3SharedLibrary.cpp"
	"src/SDL3/SDL3SharedLibrary.hpp"
	"src/SDL3/SDL3Window.cpp"
	"src/SDL3/SDL3Window.hpp"
	"src/SDL3/SDLPlatform.cpp"
)

set(PUBLIC_LIBRARY_INCLUDES
	"include"
)

set(LIBRARY_COMPILE_DEFINITIONS "NX_PLATFORM_EXPORT=1")

#platform specific configurations
if (WIN32)
  list(APPEND LIBRARY_COMPILE_DEFINITIONS NX_PLATFORM_WINDOWS=1 NOMINMAX=1 WIN32_LEAN_AND_MEAN=1)
  set(NX_PLATFORM_WGL ON CACHE BOOL "")
elseif(LINUX)
  list(APPEND LIBRARY_COMPILE_DEFINITIONS NX_PLATFORM_LINUX=1)
  set(NX_PLATFORM_EGL ON CACHE BOOL "")
elseif(EMSCRIPTEN)
  list(APPEND LIBRARY_COMPILE_DEFINITIONS NX_PLATFORM_WEB=1)
elseif(ANDROID)
  list (APPEND LIBRARY_COMPILE_DEFINITIONS NX_PLATFORM_ANDROID=1)
  set(NX_PLATFORM_EGL ON CACHE BOOL "")
endif()

add_library(Nexus-Platform ${PLATFORM_SOURCES})

target_compile_features(Nexus-Platform PRIVATE cxx_std_23)
set_target_properties(Nexus-Platform PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.4.0
)
FetchContent_MakeAvailable(SDL3)

target_link_libraries (Nexus-Platform PUBLIC Nexus-Core SDL3::SDL3)

target_compile_definitions(Nexus-Platform PUBLIC ${LIBRARY_COMPILE_DEFINITIONS})
target_include_directories(Nexus-Platform PUBLIC ${PUBLIC_LIBRARY_INCLUDES})
