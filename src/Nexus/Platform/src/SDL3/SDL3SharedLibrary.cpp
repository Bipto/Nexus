#include "SDL3SharedLibrary.hpp"

namespace Nexus::Utils
{
	SharedLibrarySDL3::SharedLibrarySDL3(const std::string &filename)
	{
		m_LibraryHandle = SDL_LoadObject(filename.c_str());
	}

	SharedLibrarySDL3::~SharedLibrarySDL3()
	{
		SDL_UnloadObject(m_LibraryHandle);
	}

	void *SharedLibrarySDL3::LoadSymbol(const std::string &symbolName) const
	{
		return SDL_LoadFunction(m_LibraryHandle, symbolName.c_str());
	}

	const std::string &SharedLibrarySDL3::GetPath() const
	{
		return m_LibraryPath;
	}
}	 // namespace Nexus::Utils