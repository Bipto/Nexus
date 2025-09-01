#include "AccelerationStructureVk.hpp"

namespace Nexus::Graphics
{
	AccelerationStructureVk::AccelerationStructureVk(const AccelerationStructureDescription &desc, GraphicsDeviceVk *device)
		: m_Description(desc),
		  m_Device(device)
	{
		Ref<DeviceBufferVk>			   destinationBuffer		 = std::dynamic_pointer_cast<DeviceBufferVk>(desc.Buffer);
		VkAccelerationStructureTypeKHR accelerationStructureType = Vk::GetAccelerationStructureType(desc.Type);

		VkAccelerationStructureCreateInfoKHR createInfo = {};
		createInfo.sType								= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.pNext								= nullptr;
		createInfo.createFlags							= 0;
		createInfo.buffer								= destinationBuffer->GetVkBuffer();
		createInfo.offset								= desc.Offset;
		createInfo.size									= desc.Size;
		createInfo.type									= accelerationStructureType;
		createInfo.deviceAddress						= 0;

		const VulkanDeviceExtensionFunctions &functions = device->GetExtensionFunctions();
		if (functions.vkCreateAccelerationStructureKHR)
		{
			NX_VALIDATE(functions.vkCreateAccelerationStructureKHR(device->GetVkDevice(), &createInfo, nullptr, &m_Handle) == VK_SUCCESS,
						"Failed to create acceleration structure");
		}

		m_Device->SetObjectName(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, (uint64_t)m_Handle, desc.DebugName.c_str());
	}

	AccelerationStructureVk::~AccelerationStructureVk()
	{
		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();
		if (functions.vkDestroyAccelerationStructureKHR)
		{
			functions.vkDestroyAccelerationStructureKHR(m_Device->GetVkDevice(), m_Handle, nullptr);
		}
	}

	const AccelerationStructureDescription &AccelerationStructureVk::GetDescription() const
	{
		return m_Description;
	}

	VkAccelerationStructureKHR AccelerationStructureVk::GetHandle() const
	{
		return m_Handle;
	}

	VkDeviceAddress AccelerationStructureVk::GetDeviceAddress() const
	{
		const VulkanDeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();

		if (functions.vkGetAccelerationStructureDeviceAddressKHR)
		{
			VkAccelerationStructureDeviceAddressInfoKHR info = {};
			info.sType										 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
			info.pNext										 = nullptr;
			info.accelerationStructure						 = m_Handle;

			return functions.vkGetAccelerationStructureDeviceAddressKHR(m_Device->GetVkDevice(), &info);
		}

		return 0;
	}
}	 // namespace Nexus::Graphics
