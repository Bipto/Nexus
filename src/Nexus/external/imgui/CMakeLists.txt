cmake_minimum_required(VERSION 3.30)

project ("ImGui")

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)

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
    "ImGuizmo/ImGuizmo.h"
    "ImGuizmo/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC imgui)
target_include_directories(imguizmo PUBLIC imgui ImGuizmo)