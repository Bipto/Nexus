#pragma once

#include <expected>
#include <string>

#include "Platform/OpenGL/GL.hpp"

#include "IGLContext.hpp"
#include "RHI/IPhysicalDevice.hpp"

#include "RHI/Fence.hpp"
#include "RHI/Sampler.hpp"
#include "RHI/TexelBuffer.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::GL
{
	class IOffscreenContext : public IGLContext
	{
	  public:
		virtual ~IOffscreenContext() = default;
	};
}	 // namespace Nexus::GL