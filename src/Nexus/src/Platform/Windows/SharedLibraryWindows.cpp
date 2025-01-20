#include "SharedLibraryWindows.hpp"

namespace Nexus::Utils
{
	SharedLibraryWindows::SharedLibraryWindows(const std::string &filepath) : SharedLibrary(filepath)
	{
		std::wstring widePath(filepath.begin(), filepath.end());
		m_Library = LoadLibraryW(widePath.c_str());
		NX_ASSERT(m_Library, "Could not load library");
	}

	SharedLibraryWindows::~SharedLibraryWindows()
	{
		FreeLibrary(m_Library);
	}

	void *SharedLibraryWindows::LoadSymbol(const std::string &name)
	{
		return GetProcAddress(m_Library, name.c_str());
	}
}	 // namespace Nexus::Utils
