##################################################################################
#-------------------------------------ImGui---------------------------------------
##################################################################################

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)

CPMAddPackage(
	NAME imgui
	GITHUB_REPOSITORY ocornut/imgui
	GIT_TAG eb642a9
	OPTIONS
		"BUILD_TESTING OFF"
)

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

##################################################################################
#-----------------------------------ImGuizmo--------------------------------------
##################################################################################

CPMAddPackage(
	NAME imguizmo
	GITHUB_REPOSITORY CedricGuillemet/ImGuizmo
	GIT_TAG b10e917
	OPTIONS
		"BUILD_TESTING OFF"
)

add_library(imguizmo
    "${imguizmo_SOURCE_DIR}/ImGuizmo.h"
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imguizmo PUBLIC ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR})