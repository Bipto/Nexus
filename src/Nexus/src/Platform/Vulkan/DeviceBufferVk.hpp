#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceVk;

	class DeviceBufferVk : public DeviceBuffer
	{
	  public:
		DeviceBufferVk(const DeviceBufferDescription &desc, GraphicsDeviceVk *device);
		virtual ~DeviceBufferVk();
		virtual void						   SetData(const void *data, uint32_t offset, uint32_t size) final;
		virtual std::vector<char>			   GetData(uint32_t offset, uint32_t size) const final;
		virtual const DeviceBufferDescription &GetDescription() const final;

		VkBuffer GetVkBuffer() const;

	  private:
		DeviceBufferDescription m_BufferDescription = {};
		Vk::AllocatedBuffer		m_Buffer;
		GraphicsDeviceVk	   *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif