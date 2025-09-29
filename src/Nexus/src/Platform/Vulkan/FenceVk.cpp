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

		const GladVulkanContext &context = m_Device->GetVulkanContext();
		NX_VALIDATE(context.CreateFence(m_Device->GetVkDevice(), &createInfo, nullptr, &m_Fence) == VK_SUCCESS, "Failed to create fence");
	}

	FenceVk::~FenceVk()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.DestroyFence(m_Device->GetVkDevice(), m_Fence, nullptr);
	}

	bool FenceVk::IsSignalled() const
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		VkResult				 result	 = context.GetFenceStatus(m_Device->GetVkDevice(), m_Fence);
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