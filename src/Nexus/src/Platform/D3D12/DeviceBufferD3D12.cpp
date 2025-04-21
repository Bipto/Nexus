#include "DeviceBufferD3D12.hpp"
#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	DeviceBufferD3D12::DeviceBufferD3D12(const DeviceBufferDescription &desc, GraphicsDeviceD3D12 *graphicsDevice) : m_BufferDescription(desc)
	{
		auto allocator = graphicsDevice->GetAllocator();

		D3D12_HEAP_TYPE heapType = D3D12::GetHeapType(desc);

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= heapType;

		D3D12_RESOURCE_DESC1 resourceDesc = {};
		resourceDesc.Dimension			  = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment			  = 0;
		resourceDesc.Width				  = desc.SizeInBytes;
		resourceDesc.Height				  = 1;
		resourceDesc.DepthOrArraySize	  = 1;
		resourceDesc.MipLevels			  = 1;
		resourceDesc.Format				  = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count	  = 1;
		resourceDesc.SampleDesc.Quality	  = 0;
		resourceDesc.Layout				  = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags				  = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = allocator->CreateResource2(&allocationDesc,
												&resourceDesc,
												D3D12_RESOURCE_STATE_COMMON,
												nullptr,
												&m_Allocation,
												IID_PPV_ARGS(&m_BufferHandle));

		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create buffer");
		}
	}

	DeviceBufferD3D12::~DeviceBufferD3D12()
	{
	}

	void DeviceBufferD3D12::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_ASSERT(m_BufferDescription.Type == DeviceBufferType::Upload, "Cannot write to a non-upload buffer");

		D3D12_RANGE range = {};
		range.Begin		  = 0;
		range.End		  = m_BufferDescription.SizeInBytes;

		void *buffer;
		m_BufferHandle->Map(0, &range, &buffer);

		void *offsetIntoBuffer = (void *)(((const char *)buffer) + offset);
		memcpy((void *)offsetIntoBuffer, data, size);

		m_BufferHandle->Unmap(0, &range);
	}

	std::vector<char> DeviceBufferD3D12::GetData(uint32_t offset, uint32_t size) const
	{
		NX_ASSERT(m_BufferDescription.Type == DeviceBufferType::Readback, "Cannot read from a non-readback buffer");
		std::vector<char> data(size);

		D3D12_RANGE range = {};
		range.Begin		  = 0;
		range.End		  = m_BufferDescription.SizeInBytes;

		void *buffer;
		m_BufferHandle->Map(0, &range, &buffer);

		void *offsetIntoBuffer = (void *)(((const char *)buffer) + offset);
		memcpy(data.data(), offsetIntoBuffer, size);

		m_BufferHandle->Unmap(0, &range);

		return data;
	}

	const DeviceBufferDescription &DeviceBufferD3D12::GetDescription() const
	{
		return m_BufferDescription;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> DeviceBufferD3D12::GetHandle()
	{
		return m_BufferHandle;
	}
}	 // namespace Nexus::Graphics
