cmake_minimum_required(VERSION 3.8)

if (WIN32)
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
	"imgui/misc/cpp/imgui_stdlib.h"
	"imgui/misc/cpp/imgui_stdlib.cpp")

add_library(imgui
	${Files}
)

add_library(imguizmo
    "imguizmo/ImGuizmo.h"
    "imguizmo/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC ../SDL/include imgui)
target_include_directories(imguizmo PUBLIC imgui)

if (WIN32)
	target_include_directories(imgui PUBLIC ${VULKAN_INCLUDE_DIRS})
	target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
endif()

if (ANDROID OR __ANDROID__)
    target_link_libraries(
        imgui
        PUBLIC
		android
		EGL
        GLESv3
		NexusEngine
    )
endif()