#include "DeviceBufferD3D12.hpp"
#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	DeviceBufferD3D12::DeviceBufferD3D12(const DeviceBufferDescription &desc, GraphicsDeviceD3D12 *graphicsDevice)
		: m_BufferDescription(desc),
		  m_GraphicsDevice(graphicsDevice)
	{
		auto allocator = graphicsDevice->GetAllocator();

		D3D12_HEAP_TYPE heapType = D3D12::GetHeapType(desc);

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= heapType;

		m_BufferSize = desc.SizeInBytes;

		// if the resource is a constant buffer, it needs to be aligned to 256 bytes
		if (desc.Usage & Graphics::BufferUsage_Uniform)
		{
			m_BufferSize = Utils::AlignTo<uint64_t>(desc.SizeInBytes, 256);
		}

		// structured/raw buffers are accessed using 4 byte alignment
		if (desc.Usage & Graphics::BufferUsage_Storage)
		{
			m_BufferSize = Utils::AlignTo<uint64_t>(desc.SizeInBytes, 4);
		}

		D3D12_RESOURCE_DESC1 resourceDesc = {};
		resourceDesc.Dimension			  = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment			  = 0;
		resourceDesc.Width				  = m_BufferSize;
		resourceDesc.Height				  = 1;
		resourceDesc.DepthOrArraySize	  = 1;
		resourceDesc.MipLevels			  = 1;
		resourceDesc.Format				  = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count	  = 1;
		resourceDesc.SampleDesc.Quality	  = 0;
		resourceDesc.Layout				  = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags				  = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.Flags				  = D3D12::GetResourceFlags(desc.Usage);

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

		std::wstring debugName = {m_BufferDescription.DebugName.begin(), m_BufferDescription.DebugName.end()};
		m_BufferHandle->SetName(debugName.c_str());
	}

	DeviceBufferD3D12::~DeviceBufferD3D12()
	{
		m_GraphicsDevice->WaitForIdle();
	}

	void DeviceBufferD3D12::SetData(const void *data, uint32_t offset, uint32_t size)
	{
		NX_VALIDATE(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Upload, "Buffer must be created on with Upload access.");

		bool	 alreadyMapped = m_MappedHandle != nullptr;
		uint8_t *dst		   = m_MappedHandle;

		if (!alreadyMapped)
		{
			dst = Map();
		}

		memcpy(dst + offset, data, size);

		if (!alreadyMapped)
		{
			Unmap();
		}
	}

	std::vector<char> DeviceBufferD3D12::GetData(uint32_t offset, uint32_t size)
	{
		NX_VALIDATE(m_BufferDescription.Access == Graphics::BufferMemoryAccess::Readback, "Buffer must be created on with Readnack access.");
		std::vector<char> data(size);

		bool	 alreadyMapped = m_MappedHandle != nullptr;
		uint8_t *dst		   = m_MappedHandle;

		if (!alreadyMapped)
		{
			dst = Map();
		}

		memcpy(data.data(), dst + offset, size);

		if (!alreadyMapped)
		{
			Unmap();
		}

		return data;
	}

	const DeviceBufferDescription &DeviceBufferD3D12::GetDescription() const
	{
		return m_BufferDescription;
	}

	DeviceAddress DeviceBufferD3D12::GetDeviceAddress(size_t offset) const
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = m_BufferHandle->GetGPUVirtualAddress();
		address += offset;
		return address;
	}

	[[nodiscard]] uint8_t *DeviceBufferD3D12::Map()
	{
		// we can only map an upload or readback buffer
		if (m_BufferDescription.Access == BufferMemoryAccess::Default)
		{
			return nullptr;
		}

		// if the buffer is already mapped, we can directly return this pointer
		if (m_MappedHandle)
		{
			return m_MappedHandle;
		}

		D3D12_RANGE range = {};
		range.Begin		  = 0;
		range.End		  = m_BufferDescription.SizeInBytes;

		void *buffer;
		m_BufferHandle->Map(0, &range, &buffer);
		m_MappedHandle = reinterpret_cast<uint8_t *>(buffer);

		return m_MappedHandle;
	}

	void DeviceBufferD3D12::Unmap()
	{
		if (m_BufferHandle)
		{
			D3D12_RANGE range = {};
			range.Begin		  = 0;
			range.End		  = m_BufferDescription.SizeInBytes;

			m_BufferHandle->Unmap(0, &range);
			m_MappedHandle = nullptr;
		}
	}

	void DeviceBufferD3D12::FlushRange(BufferRange range)
	{
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> DeviceBufferD3D12::GetHandle()
	{
		return m_BufferHandle;
	}

	size_t DeviceBufferD3D12::GetBufferSizeInBytes()
	{
		return m_BufferSize;
	}
}	 // namespace Nexus::Graphics
