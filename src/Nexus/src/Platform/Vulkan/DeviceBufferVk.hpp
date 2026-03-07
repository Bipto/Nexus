#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "RHI/DeviceBuffer.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceVk;

	class DeviceBufferVk : public IDeviceBuffer
	{
	  public:
		DeviceBufferVk(const DeviceBufferDescription &desc, GraphicsDeviceVk *device);
		virtual ~DeviceBufferVk();
		void						   SetData(const void *data, uint32_t offset, uint32_t size) final;
		std::vector<char>			   GetData(uint32_t offset, uint32_t size) final;
		const DeviceBufferDescription &GetDescription() const final;
		DeviceAddress				   GetDeviceAddress(size_t offset) const final;

		[[nodiscard]] uint8_t *Map() final;
		void				   Unmap() final;
		void				   FlushRange(BufferRange range) final;

		VkBuffer		GetVkBuffer() const;
		VkDeviceAddress GetVkDeviceAddress() const;

	  private:
		DeviceBufferDescription m_BufferDescription = {};
		Vk::AllocatedBuffer		m_Buffer;
		GraphicsDeviceVk	   *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif