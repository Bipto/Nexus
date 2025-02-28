#include "Nexus-Core/Utils/ScriptProjectGenerator.hpp"
#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::Utils
{
	const char *scriptCmakeText = R"(cmake_minimum_required(VERSION 3.10)

project(SCRIPT_PROJECT_NAME)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 2d4c4b4
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 2f91176
)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
  FetchContent_MakeAvailable(imgui)

  set (IMGUI_INCLUDE_DIR "${imgui_SOURCE_DIR}/")

  add_library(imgui STATIC
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
	  "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
  )

  target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR})
endif()

FetchContent_Declare(
  imguizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
  GIT_TAG b10e917
)
FetchContent_GetProperties(imguizmo)
if (NOT imguizmo_POPULATED)
  FetchContent_Populate(imguizmo)

  set (IMGUIZMO_INCLUDE_DIR "${imguizmo_SOURCE_DIR}/")

  add_library(imguizmo STATIC
    "${imguizmo_SOURCE_DIR}/ImGuizmo.h"
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp")

  target_include_directories(imguizmo PUBLIC ${IMGUI_INCLUDE_DIR} ${imguizmo_SOURCE_DIR})
endif()


FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 39f7374
)
FetchContent_MakeAvailable(yaml-cpp)


FetchContent_Declare(
  pocketpy
  GIT_REPOSITORY https://github.com/pocketpy/pocketpy.git
  GIT_TAG 3f5c460
)
FetchContent_MakeAvailable(pocketpy)

add_library(SCRIPT_PROJECT_NAME SHARED main.cpp)
target_include_directories(SCRIPT_PROJECT_NAME PRIVATE Nexus/include ${pocketpy_SOURCE_DIR}/include)
target_link_directories(SCRIPT_PROJECT_NAME PRIVATE ${CMAKE_SOURCE_DIR} Nexus/lib)
target_link_libraries(SCRIPT_PROJECT_NAME PRIVATE Nexusd glm::glm imgui imguizmo yaml-cpp::yaml-cpp pocketpy)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/loading)

add_custom_command(
	TARGET SCRIPT_PROJECT_NAME POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	$<TARGET_FILE:SCRIPT_PROJECT_NAME>
	${CMAKE_SOURCE_DIR}/loading/${CMAKE_BUILD_TYPE}/$<TARGET_FILE_NAME:SCRIPT_PROJECT_NAME>)

add_custom_command(
	TARGET SCRIPT_PROJECT_NAME POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_SOURCE_DIR}/loading/${CMAKE_BUILD_TYPE}/$<TARGET_FILE_NAME:SCRIPT_PROJECT_NAME>" > ${CMAKE_SOURCE_DIR}/loading/output.txt
)

)";

	const char *scriptMainText = R"(#include <iostream>

void say_hello()
{
    std::cout << "Hello World" << std::endl;
}
)";

	void ScriptProjectGenerator::Generate(const std::string &templatePath, const std::string &projectName, const std::string &projectDirectory)
	{
		std::string scriptDirectory = projectDirectory + std::string("\\") + projectName + std::string("\\Scripts");
		FileSystem::CreateFileDirectory(scriptDirectory);

		// setup script project
		{
			std::string scriptCmakeFile			 = scriptDirectory + "\\CMakeLists.txt";
			std::string scriptProcessedCmakeText = StringUtils::Replace(scriptCmakeText, "SCRIPT_PROJECT_NAME", projectName);
			FileSystem::WriteFile(scriptCmakeFile, scriptProcessedCmakeText);

			std::string scriptMainFile = scriptDirectory + "\\main.cpp";
			FileSystem::WriteFile(scriptMainFile, scriptMainText);
		}

		// copy Nexus includes and libs
		{
			std::string fromDir = "Nexus";
			std::string toDir	= scriptDirectory + "\\Nexus";

			if (std::filesystem::exists(fromDir))
			{
				std::cout << "Copying directory" << std::endl;
				FileSystem::CreateFileDirectory(toDir);
				FileSystem::CopyDirectory(fromDir, toDir, true);
			}
		}
	}
}	 // namespace Nexus::Utils