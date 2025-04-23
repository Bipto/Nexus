#include "DeviceBufferVk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
	DeviceBufferVk::DeviceBufferVk(const DeviceBufferDescription &desc, GraphicsDeviceVk *device) : m_BufferDescription(desc), m_Device(device)
	{
		VkBufferCreateInfo		bufferCreateInfo = Vk::GetVkBufferCreateInfo(desc);
		VmaAllocationCreateInfo vmaAllocInfo	 = Vk::GetVmaAllocationCreateInfo(desc);

		NX_ASSERT(vmaCreateBuffer(device->GetAllocator(), &bufferCreateInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr) ==
					  VK_SUCCESS,
				  "Failed to create buffer");
	}

	DeviceBufferVk::~DeviceBufferVk()
	{
		vmaDestroyBuffer(m_Device->GetAllocator(), m_Buffer.Buffer, m_Buffer.Allocation);
	}

	void DeviceBufferVk::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_ASSERT(m_BufferDescription.Type == DeviceBufferType::Upload || m_BufferDescription.HostVisible,
				  "Buffer must be an upload buffer or have been created with the HostVisible property set");

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
		NX_ASSERT(m_BufferDescription.Type == DeviceBufferType::Readback || m_BufferDescription.HostVisible,
				  "Buffer must be a readback buffer or have been created with the HostVisible property set");

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
}	 // namespace Nexus::Graphics
