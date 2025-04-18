#include "DeviceBufferVk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
	DeviceBufferVk::DeviceBufferVk(const DeviceBufferDescription &desc, GraphicsDeviceVk *device) : m_BufferDescription(desc), m_Device(device)
	{
	}

	DeviceBufferVk::~DeviceBufferVk()
	{
	}

	void DeviceBufferVk::SetData(const void *data, uint32_t offset, uint32_t size)
	{
	}

	std::vector<char> DeviceBufferVk::GetData(uint32_t offset, uint32_t size) const
	{
		return std::vector<char>();
	}

	const DeviceBufferDescription &DeviceBufferVk::GetDescription() const
	{
		return m_BufferDescription;
	}
}	 // namespace Nexus::Graphics
