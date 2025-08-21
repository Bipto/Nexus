cmake_minimum_required(VERSION 3.28)

project ("ImGui")

include(FetchContent)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG eb642a9
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
	imguizmo
	GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
	GIT_TAG b10e917
)
FetchContent_MakeAvailable(imguizmo)

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)

set (Files
	"${imgui_SOURCE_DIR}/imconfig.h"
	"${imgui_SOURCE_DIR}/imgui.cpp"
	"${imgui_SOURCE_DIR}/imgui.h"
	"${imgui_SOURCE_DIR}/imgui_demo.cpp"
	"${imgui_SOURCE_DIR}/imgui_draw.cpp"
	"${imgui_SOURCE_DIR}/imgui_internal.h"
	"${imgui_SOURCE_DIR}/imgui_tables.cpp"
	"${imgui_SOURCE_DIR}/imgui_widgets.cpp"
	"${imgui_SOURCE_DIR}/imstb_rectpack.h"
	"${imgui_SOURCE_DIR}/imstb_textedit.h"
	"${imgui_SOURCE_DIR}/imstb_truetype.h"
	"${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
	"${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp")

add_library(imgui
	${Files}
)

add_library(imguizmo
    "${imguizmo_SOURCE_DIR}/ImGuizmo.h"
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}")
target_include_directories(imguizmo PUBLIC "${imgui_SOURCE_DIR}" "${imguizmo_SOURCE_DIR}")