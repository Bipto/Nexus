#include "SharedLibraryUnix.hpp"

namespace Nexus::Utils
{
	SharedLibraryUnix::SharedLibraryUnix(const std::string &filepath)
	{
	}

	SharedLibraryUnix::~SharedLibraryUnix()
	{
	}

	void *SharedLibraryUnix::LoadSymbol(const std::string &name)
	{
		return nullptr;
	}

}	 // namespace Nexus::Utils
