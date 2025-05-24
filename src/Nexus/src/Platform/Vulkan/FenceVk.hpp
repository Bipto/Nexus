#pragma once

#include "Nexus-Core/Graphics/Fence.hpp"
#include "Platform/Vulkan/Vk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
	class FenceVk : public Fence
	{
	  public:
		FenceVk(const FenceDescription &desc, GraphicsDeviceVk *device);
		virtual ~FenceVk();

		bool					IsSignalled() const final;
		const FenceDescription &GetDescription() const final;

		VkFence GetHandle();

	  private:
		FenceDescription  m_Description = {};
		VkFence			  m_Fence		= nullptr;
		GraphicsDeviceVk *m_Device		= nullptr;
	};
}	 // namespace Nexus::Graphics