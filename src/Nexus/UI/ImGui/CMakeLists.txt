add_library(Nexus-UI-ImGui 
	"include/ImGui/ImGuiLayout.hpp"
	"include/ImGui/ImGuiPanel.hpp"
	"include/ImGui/ImGuiMenubar.hpp"
	"include/ImGui/ImGuiStatusBar.hpp"
	"include/ImGui/ImGuiButton.hpp"
	"src/ImGui/ImGuiLayout.cpp"
	"src/ImGui/ImGuiPanel.cpp"
	"src/ImGui/ImGuiMenubar.cpp"
	"src/ImGui/ImGuiStatusBar.cpp"
	"src/ImGui/ImGuiButton.cpp"
	"src/ImGui/ImGuiFrame.cpp")

target_include_directories(Nexus-UI-ImGui  PUBLIC "include")

set_target_properties(Nexus-UI-ImGui  PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

target_compile_features(Nexus-UI-ImGui  PUBLIC cxx_std_23)
target_link_libraries(Nexus-UI-ImGui  PUBLIC Nexus-Core Nexus-UI)
target_link_libraries(Nexus-UI-ImGui  PUBLIC imgui imguizmo)