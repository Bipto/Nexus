#include "DeviceBufferD3D12.hpp"
#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	DeviceBufferD3D12::DeviceBufferD3D12(const DeviceBufferDescription &desc, GraphicsDeviceD3D12 *graphicsDevice) : m_BufferDescription(desc)
	{
		auto d3d12Device = graphicsDevice->GetDevice();

		D3D12_HEAP_TYPE heapType = D3D12::GetHeapType(desc);

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= heapType;
	}

	DeviceBufferD3D12::~DeviceBufferD3D12()
	{
	}

	void DeviceBufferD3D12::SetData(const void *data, uint32_t offset, uint32_t size)
	{
	}

	std::vector<char> DeviceBufferD3D12::GetData(uint32_t offset, uint32_t size) const
	{
		return std::vector<char>();
	}

	const DeviceBufferDescription &DeviceBufferD3D12::GetDescription() const
	{
		return m_BufferDescription;
	}
}	 // namespace Nexus::Graphics
