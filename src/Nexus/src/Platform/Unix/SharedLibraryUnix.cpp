#include "SharedLibraryUnix.hpp"

namespace Nexus::Utils
{
	SharedLibraryUnix::SharedLibraryUnix(const std::string &filepath) : SharedLibrary(filepath)
	{
		m_ModuleHandle = dlopen(filepath.c_str(), RTLD_LAZY);
	}

	SharedLibraryUnix::~SharedLibraryUnix()
	{
		dlclose(m_ModuleHandle);
	}

	void *SharedLibraryUnix::LoadSymbol(const std::string &name)
	{
		return dlsym(m_ModuleHandle, name.c_str());
	}

}	 // namespace Nexus::Utils
