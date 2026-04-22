add_library(Nexus-UI-wxWidgets 
	"include/wxWidgets/wxWidgetsLayout.hpp"
	"include/wxWidgets/wxWidgetsPanel.hpp"
	"include/wxWidgets/wxWidgetsMenubar.hpp"
	"src/wxWidgets/wxWidgetsLayout.cpp"
	"src/wxWidgets/wxWidgetsPanel.cpp"
	"src/wxWidgets/wxWidgetsMenubar.cpp"
)

CPMAddPackage(
	NAME wxWidgets
	GITHUB_REPOSITORY wxWidgets/wxWidgets
	GIT_TAG v3.3.2
        "wxBUILD_SHARED OFF"
        "wxBUILD_TESTS OFF"
)

target_include_directories(Nexus-UI-wxWidgets PUBLIC "include")

set_target_properties(Nexus-UI-wxWidgets PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

target_compile_features(Nexus-UI-wxWidgets PUBLIC cxx_std_23)
target_link_libraries(Nexus-UI-wxWidgets PUBLIC Nexus-Core Nexus-UI)
target_link_libraries(Nexus-UI-wxWidgets PUBLIC wx::net wx::core wx::base wx::aui)