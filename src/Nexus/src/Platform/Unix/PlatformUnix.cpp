#include "Platform/Unix/UnixInclude.hpp"

#include "Platform/Platform.hpp"
#include "SharedLibraryUnix.hpp"

namespace Nexus::Platform
{
	Utils::SharedLibrary *LoadSharedLibrary(const std::string &filename)
	{
		return new Utils::SharedLibraryUnix(filename.c_str());
	}
}	 // namespace Nexus::Platform