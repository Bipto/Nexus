cmake_minimum_required(VERSION 3.30)

project ("Nexus-RHI")

include (FetchContent)

set(RHI_SOURCES 
	)

set(PUBLIC_LIBRARY_INCLUDES
	"include"
)

set(PRIVATE_LIBRARY_INCLUDES "")

set(LIBRARY_COMPILE_DEFINITIONS "NX_RHI_EXPORT=1")

add_library(Nexus-RHI ${AUDIO_SOURCES})

target_compile_features(Nexus-RHI PUBLIC cxx_std_23)
set_target_properties(Nexus-RHI PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON)

target_compile_definitions(Nexus-RHI PUBLIC ${LIBRARY_COMPILE_DEFINITIONS})
target_include_directories(Nexus-RHI PUBLIC ${PUBLIC_LIBRARY_INCLUDES})
target_include_directories(Nexus-RHI PRIVATE ${PRIVATE_LIBRARY_INCLUDES})
target_link_libraries(Nexus-RHI PUBLIC ${NEXUS_LINKED_LIBRARIES})