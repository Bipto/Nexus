#pragma once

#include "Platform/Windows/WindowsInclude.hpp"

#include "Nexus-Core/Platform.hpp"
#include "SharedLibraryWindows.hpp"

namespace Nexus::Platform
{
	Utils::SharedLibrary *LoadSharedLibrary(const std::string &filename)
	{
		return new Utils::SharedLibraryWindows(filename.c_str());
	}
}	 // namespace Nexus::Platform