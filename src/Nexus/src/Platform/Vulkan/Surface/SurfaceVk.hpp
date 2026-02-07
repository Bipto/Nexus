#pragma once

#include "Nexus-Core/Graphics/ISurface.hpp"

#include "../Vk.hpp"
#include "Nexus-Core/Graphics/SwapchainDescription.hpp"

namespace Nexus::Graphics
{
	class SurfaceVk : public ISurface
	{
	  public:
		virtual ~SurfaceVk()																				 = default;
		virtual tl::expected<VkSurfaceKHR, VkResult> CreateVkSurface(const SwapchainDescription &swapchainDesc,
																	 VkInstance					 instance,
																	 const GladVulkanContext	&context) const = 0;
	};
}	 // namespace Nexus::Graphics