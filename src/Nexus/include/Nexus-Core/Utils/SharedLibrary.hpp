#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Utils
{
	class SharedLibrary
	{
	  public:
		SharedLibrary(const std::string &filepath) : m_LibraryPath(filepath)
		{
		}

		virtual ~SharedLibrary()
		{
		}

		virtual void *LoadSymbol(const std::string &name) = 0;

	  protected:
		std::string m_LibraryPath = {};
	};
}	 // namespace Nexus::Utils