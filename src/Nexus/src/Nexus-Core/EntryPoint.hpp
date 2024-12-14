#pragma once

#include "Application.hpp"
#include "Runtime.hpp"

#if defined(NX_PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

/// @brief A typedef to simplify using command line arguments
typedef std::vector<std::string> CommandLineArguments;

namespace Nexus
{
	/// @brief A method that creates an application and is required to be created by
	/// the client
	/// @param arguments A vector of strings containing the arguments that were
	/// entered to the application
	/// @return A pointer to a created application
	extern Application *CreateApplication(const CommandLineArguments &arguments);

	/// @brief A method representing the main entry point of the application
	/// @param arguments A const reference to the options passed to the application
	/// @return An integer value representing whether the application was able to
	/// execute successfully
	int Main(const CommandLineArguments &arguments)
	{
		Nexus::Init();

		Application *app = CreateApplication(arguments);
		Nexus::Run(app);
		delete app;

		Nexus::Shutdown();

		return 0;
	}
}	 // namespace Nexus

namespace
{
	const std::vector<std::string> InputParametersToStringVector(int argc, char **argv)
	{
		std::vector<std::string> commandLineArgs;
		for (int i = 0; i < argc; i++) { commandLineArgs.push_back(argv[i]); }
		return commandLineArgs;
	}
}	 // namespace

#if !defined(NX_NO_ENTRY_POINT)
	#if defined(NX_NO_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	auto commandLineArgs = InputParametersToStringVector(__argc, __argv);
	return Nexus::Main(commandLineArgs);
}
	#else
int main(int argc, char **argv)
{
	auto commandLineArgs = InputParametersToStringVector(argc, argv);
	return Nexus::Main(commandLineArgs);
}
	#endif
#endif