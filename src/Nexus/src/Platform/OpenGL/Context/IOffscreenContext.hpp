#pragma once

#include "IGLContext.hpp"
#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"

namespace Nexus::GL
{
	class IOffscreenContext : public IGLContext
	{
	  public:
		virtual ~IOffscreenContext()
		{
		}
	};
}	 // namespace Nexus::GL