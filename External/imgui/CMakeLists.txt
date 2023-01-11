cmake_minimum_required(VERSION 3.8)

project ("ImGui")

add_library(imgui
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
	"imgui/backends/imgui_impl_sdl.h"	
	"imgui/backends/imgui_impl_sdl.cpp"	
)

add_library(imguizmo
    "imguizmo/ImGuizmo.h"
    "imguizmo/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC ../SDL/include)
target_include_directories(imgui PUBLIC imgui)

target_include_directories(imguizmo PUBLIC imgui)