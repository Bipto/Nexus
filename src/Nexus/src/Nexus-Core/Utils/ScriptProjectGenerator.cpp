#include "Nexus-Core/Utils/ScriptProjectGenerator.hpp"
#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::Utils
{
	const char *scriptCmakeText = R"(cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


project(SCRIPT_PROJECT_NAME)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

add_definitions(-DNX_PLATFORM_WINDOWS=1)
add_definitions(-DNX_EXPORT_API=1)

add_library(SCRIPT_PROJECT_NAME SHARED main.cpp)

set(NX_BUILD_TYPE INTERFACE)

include (FetchContent)
FetchContent_Declare(
  Nexus
  GIT_REPOSITORY https://github.com/Bipto/Nexus.git
  GIT_TAG dev
)
FetchContent_MakeAvailable(Nexus)
target_link_libraries(SCRIPT_PROJECT_NAME PRIVATE Nexus)

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
		FileSystem::CreateDirectory(scriptDirectory);

		// setup script project
		{
			std::string scriptCmakeFile			 = scriptDirectory + "\\CMakeLists.txt";
			std::string scriptProcessedCmakeText = StringUtils::Replace(scriptCmakeText, "SCRIPT_PROJECT_NAME", projectName);
			FileSystem::WriteFile(scriptCmakeFile, scriptProcessedCmakeText);

			std::string scriptMainFile = scriptDirectory + "\\main.cpp";
			FileSystem::WriteFile(scriptMainFile, scriptMainText);
		}
	}
}	 // namespace Nexus::Utils
