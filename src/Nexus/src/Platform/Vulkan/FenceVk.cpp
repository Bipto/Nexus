#include "FenceVk.hpp"

namespace Nexus::Graphics
{
	FenceVk::FenceVk(const FenceDescription &desc, GraphicsDeviceVk *device) : m_Description(desc), m_Device(device)
	{
		VkFenceCreateInfo createInfo = {};
		createInfo.sType			 = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.pNext			 = nullptr;
		createInfo.flags			 = 0;

		if (desc.Signalled)
		{
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		}

		NX_VALIDATE(vkCreateFence(m_Device->GetVkDevice(), &createInfo, nullptr, &m_Fence) == VK_SUCCESS, "Failed to create fence");
	}

	FenceVk::~FenceVk()
	{
		vkDestroyFence(m_Device->GetVkDevice(), m_Fence, nullptr);
	}

	bool FenceVk::IsSignalled() const
	{
		VkResult result = vkGetFenceStatus(m_Device->GetVkDevice(), m_Fence);
		return result == VK_SUCCESS;
	}

	const FenceDescription &FenceVk::GetDescription() const
	{
		return m_Description;
	}

	VkFence FenceVk::GetHandle()
	{
		return m_Fence;
	}
}	 // namespace Nexus::Graphics