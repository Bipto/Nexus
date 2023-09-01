cmake_minimum_required(VERSION 3.8)

if (NOT DEFINED EMSCRIPTEN)
find_package(Vulkan REQUIRED)
endif()

project ("ImGui")

set (Files
	"imgui/imconfig.h"
	"imgui/imgui.cpp"
	"imgui/imgui.h"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_internal.h"
	"imgui/imgui_tables.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"
	"imgui/backends/imgui_impl_opengl3.h"
	"imgui/backends/imgui_impl_opengl3.cpp"
	"imgui/backends/imgui_impl_sdl2.h"	
	"imgui/backends/imgui_impl_sdl2.cpp"	
	"imgui/misc/cpp/imgui_stdlib.h"
	"imgui/misc/cpp/imgui_stdlib.cpp")

if (WIN32)
	list(APPEND Files "imgui/backends/imgui_impl_dx11.h")
	list(APPEND Files "imgui/backends/imgui_impl_dx11.cpp")
	list(APPEND Files "imgui/backends/imgui_impl_vulkan.h")
	list(APPEND Files "imgui/backends/imgui_impl_vulkan.cpp")
endif()

add_library(imgui
	${Files}
)

add_library(imguizmo
    "imguizmo/ImGuizmo.h"
    "imguizmo/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC ../SDL/include imgui)
target_include_directories(imguizmo PUBLIC imgui)

if (NOT DEFINED EMSCRIPTEN)
	target_include_directories(imgui PUBLIC ${VULKAN_INCLUDE_DIRS})
	target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
endif()