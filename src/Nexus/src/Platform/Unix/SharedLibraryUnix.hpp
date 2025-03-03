#pragma once

#include "Nexus-Core/Utils/SharedLibrary.hpp"
#include "Platform/Unix/UnixInclude.hpp"

namespace Nexus::Utils
{
	class SharedLibraryUnix : public SharedLibrary
	{
	  public:
		SharedLibraryUnix(const std::string &filepath);
		virtual ~SharedLibraryUnix();
		void *LoadSymbol(const std::string &name) final;

	  private:
		void *m_ModuleHandle = nullptr;
	};
}	 // namespace Nexus::Utils