#include "Nexus-Core/Utils/ScriptProjectGenerator.hpp"
#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Utils/StringUtils.hpp"

namespace Nexus::Utils
{
	const char *engineCmakeText = R"(cmake_minimum_required(VERSION 3.10)
project(Nexus)

add_library(Nexus INTERFACE)
target_include_directories(Nexus INTERFACE include/))";

	const char *scriptCmakeText = R"(cmake_minimum_required(VERSION 3.10)
project(SCRIPT_PROJECT_NAME)

add_subdirectory(Nexus)

add_library(SCRIPT_PROJECT_NAME SHARED main.cpp)
target_link_libraries(SCRIPT_PROJECT_NAME PRIVATE Nexus)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT SCRIPT_PROJECT_NAME)

add_custom_command(
	TARGET SCRIPT_PROJECT_NAME POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:SCRIPT_PROJECT_NAME>" > ${CMAKE_SOURCE_DIR}/output.txt
)

add_custom_command(
	TARGET SCRIPT_PROJECT_NAME POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_PDB_FILE:SCRIPT_PROJECT_NAME>" > ${CMAKE_SOURCE_DIR}/pdb_path.txt
))";

	const char *scriptMainText = R"(#include <iostream>

void say_hello()
{
    std::cout << "Hello World" << std::endl;
}
)";

	void ScriptProjectGenerator::Generate(const std::string &includeFilePath, const std::string &projectName, const std::string &projectDirectory)
	{
		std::string scriptDirectory = projectDirectory + std::string("\\") + projectName + std::string("\\Scripts");
		FileSystem::CreateDirectory(scriptDirectory);

		// setup engine
		{
			std::string engineDirectory		   = scriptDirectory + "\\Nexus";
			std::string engineIncludeDirectory = engineDirectory + "\\include";
			FileSystem::CreateDirectory(engineIncludeDirectory);
			FileSystem::CopyDirectory(includeFilePath, engineIncludeDirectory, true);

			std::string engineCmakeFile = engineDirectory + "\\CMakeLists.txt";
			FileSystem::WriteFile(engineCmakeFile, engineCmakeText);
		}

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
