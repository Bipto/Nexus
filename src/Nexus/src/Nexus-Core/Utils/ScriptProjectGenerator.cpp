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

add_definitions(-DNX_PLATFORM_WINDOWS=1)
add_definitions(-DNX_EXPORT_API=1)

add_library(SCRIPT_PROJECT_NAME SHARED main.cpp)

#target_include_directories(SCRIPT_PROJECT_NAME PRIVATE external/Nexus/src/Nexus/include)
#target_link_libraries(SCRIPT_PROJECT_NAME PRIVATE Nexus)
#add_dependencies(SCRIPT_PROJECT_NAME Nexus)
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

		// setup engine
		{
			std::string engineDirectory = scriptDirectory + "\\Nexus";
			FileSystem::CreateDirectory(engineDirectory);
			FileSystem::CopyDirectory(templatePath, engineDirectory, true);
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
