#pragma once

#include <string>

namespace Nexus::Utils
{
	class SharedLibrary
	{
	  public:
		SharedLibrary(const std::string &filepath) : m_LibraryPath(filepath)
		{
		}
		virtual ~SharedLibrary()						  = default;
		virtual void *LoadSymbol(const std::string &name) = 0;

	  protected:
		std::string m_LibraryPath = {};
	};
}	 // namespace Nexus::Utils