#pragma once

#include "Nexus-Core/Utils/SharedLibrary.hpp"
#include "Platform/Windows/WindowsInclude.hpp"

namespace Nexus::Utils
{
	class SharedLibraryWindows : public SharedLibrary
	{
	  public:
		SharedLibraryWindows(const std::string &filepath);
		virtual ~SharedLibraryWindows();
		void *LoadSymbol(const std::string &name) final;

	  private:
		HINSTANCE m_Library = nullptr;
	};
}	 // namespace Nexus::Utils