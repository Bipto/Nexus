#pragma once

#include <SDL3/SDL_loadso.h>

#include "Platform/Utils/SharedLibrary.hpp"

namespace Nexus::Utils
{
	class SharedLibrarySDL3 final : public SharedLibrary
	{
	  public:
		SharedLibrarySDL3(const std::string &filename);
		~SharedLibrarySDL3() override;
		FunctionPointer			   LoadSymbol(const std::string &symbolName) const final;
		virtual const std::string &GetPath() const final;

	  private:
		SDL_SharedObject *m_LibraryHandle = nullptr;
		std::string		  m_LibraryPath	  = {};
	};
}	 // namespace Nexus::Utils