#include "DeviceBufferVk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
	DeviceBufferVk::DeviceBufferVk(const DeviceBufferDescription &desc, GraphicsDeviceVk *device) : m_BufferDescription(desc), m_Device(device)
	{
		VkBufferCreateInfo		bufferCreateInfo = Vk::GetVkBufferCreateInfo(desc, device);
		VmaAllocationCreateInfo vmaAllocInfo	 = Vk::GetVmaAllocationCreateInfo(desc, device);

		NX_VALIDATE(vmaCreateBuffer(device->GetAllocator(), &bufferCreateInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) ==
						VK_SUCCESS,
					"Failed to create buffer");

		device->SetObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)m_Buffer.Buffer, desc.DebugName.c_str());
	}

	DeviceBufferVk::~DeviceBufferVk()
	{
		vmaDestroyBuffer(m_Device->GetAllocator(), m_Buffer.Buffer, m_Buffer.Allocation);
	}

	void DeviceBufferVk::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_VALIDATE(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Upload, "Buffer must have been created with Upload access");

		void *buffer;
		vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
		{
			void *offsetIntoBuffer = (void *)((const char *)buffer + offset);
			memcpy(offsetIntoBuffer, data, size);
		}

		vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);
	}

	std::vector<char> DeviceBufferVk::GetData(uint32_t offset, uint32_t size) const
	{
		NX_VALIDATE(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Readback, "Buffer must have been created with Readback access");

		std::vector<char> data(size);

		void *buffer;
		vmaMapMemory(m_Device->GetAllocator(), m_Buffer.Allocation, &buffer);
		{
			void *offsetIntoBuffer = (void *)((const char *)buffer + offset);
			memcpy(data.data(), offsetIntoBuffer, data.size());
		}
		vmaUnmapMemory(m_Device->GetAllocator(), m_Buffer.Allocation);

		return data;
	}

	const DeviceBufferDescription &DeviceBufferVk::GetDescription() const
	{
		return m_BufferDescription;
	}

	VkBuffer DeviceBufferVk::GetVkBuffer() const
	{
		return m_Buffer.Buffer;
	}

	VkDeviceAddress DeviceBufferVk::GetDeviceAddress() const
	{
		const DeviceExtensionFunctions &functions = m_Device->GetExtensionFunctions();
		if (functions.vkGetBufferDeviceAddressKHR)
		{
			VkBufferDeviceAddressInfo info = {};
			info.sType					   = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			info.pNext					   = nullptr;
			info.buffer					   = m_Buffer.Buffer;

			return functions.vkGetBufferDeviceAddressKHR(m_Device->GetVkDevice(), &info);
		}

		return {};
	}
}	 // namespace Nexus::Graphics
