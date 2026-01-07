#pragma once

#include <string>

#include "Platform/Platform-Core.hpp"

namespace Nexus::Utils
{
	class NX_PLATFORM_API SharedLibrary
	{
	  public:
		virtual ~SharedLibrary()												   = default;
		virtual void			  *LoadSymbol(const std::string &symbolName) const = 0;
		virtual const std::string &GetPath() const								   = 0;
	};
}	 // namespace Nexus::Utils