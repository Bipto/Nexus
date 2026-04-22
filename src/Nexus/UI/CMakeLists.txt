add_library(Nexus-UI INTERFACE 
	"include/UI/Control.hpp"	
	"include/UI/Layout.hpp"	
	"include/UI/Menubar.hpp"	
	"include/UI/Panel.hpp"	
)

target_include_directories(Nexus-UI INTERFACE "include")

set_target_properties(Nexus-UI PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

target_compile_features(Nexus-UI INTERFACE cxx_std_23)
target_compile_definitions(Nexus-UI INTERFACE NX_UI_EXPORT=1)
target_link_libraries(Nexus-UI INTERFACE Nexus-Core)

add_subdirectory(wxWidgets)