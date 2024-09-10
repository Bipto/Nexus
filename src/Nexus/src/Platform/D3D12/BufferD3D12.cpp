#include "BufferD3D12.hpp"

#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

namespace Nexus::Graphics
{
	VertexBufferD3D12::VertexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data)
		: VertexBuffer(description, data),
		  m_Device(device)
	{
		auto d3d12Device = device->GetDevice();

		D3D12_HEAP_PROPERTIES uploadProperties;
		uploadProperties.Type				  = D3D12_HEAP_TYPE_UPLOAD;
		uploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadProperties.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadProperties.CreationNodeMask	  = 0;
		uploadProperties.VisibleNodeMask	  = 0;

		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment			= D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		uploadBufferDesc.Width				= m_Description.Size;
		uploadBufferDesc.Height				= 1;
		uploadBufferDesc.DepthOrArraySize	= 1;
		uploadBufferDesc.MipLevels			= 1;
		uploadBufferDesc.Format				= DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count	= 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags				= D3D12_RESOURCE_FLAG_NONE;

		d3d12Device->CreateCommittedResource(&uploadProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &uploadBufferDesc,
											 D3D12_RESOURCE_STATE_COMMON,
											 nullptr,
											 IID_PPV_ARGS(&m_UploadBuffer));

		D3D12_HEAP_PROPERTIES bufferProperties;
		bufferProperties.Type				  = D3D12_HEAP_TYPE_DEFAULT;
		bufferProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		bufferProperties.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		bufferProperties.CreationNodeMask	  = 0;
		bufferProperties.VisibleNodeMask	  = 0;

		D3D12_RESOURCE_DESC bufferDesc;
		bufferDesc.Dimension		  = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment		  = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		bufferDesc.Width			  = m_Description.Size;
		bufferDesc.Height			  = 1;
		bufferDesc.DepthOrArraySize	  = 1;
		bufferDesc.MipLevels		  = 1;
		bufferDesc.Format			  = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count	  = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout			  = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags			  = D3D12_RESOURCE_FLAG_NONE;
		d3d12Device->CreateCommittedResource(&bufferProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &bufferDesc,
											 D3D12_RESOURCE_STATE_COMMON,
											 nullptr,
											 IID_PPV_ARGS(&m_VertexBuffer));

		m_UploadRange.Begin = 0;
		m_UploadRange.End	= m_Description.Size;

		/* m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = m_Description.Size;
		m_VertexBufferView.StrideInBytes = m_Layout.GetStride(); */

		if (!data)
			return;

		SetData(data, m_Description.Size, 0);
	}

	VertexBufferD3D12::~VertexBufferD3D12()
	{
	}

	void VertexBufferD3D12::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		void *buffer;
		m_UploadBuffer->Map(0, &m_UploadRange, &buffer);

		void *offsetIntoBuffer = (void *)(((const char *)buffer) + offset);
		memcpy((void *)offsetIntoBuffer, data, size);

		m_UploadBuffer->Unmap(0, &m_UploadRange);
		m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd)
								  { cmd->CopyBufferRegion(m_VertexBuffer.Get(), 0, m_UploadBuffer.Get(), 0, m_Description.Size); });
	}

	ID3D12Resource2 *VertexBufferD3D12::GetHandle()
	{
		return m_VertexBuffer.Get();
	}

	IndexBufferD3D12::IndexBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data, IndexBufferFormat format)
		: IndexBuffer(description, data, format),
		  m_Device(device)
	{
		auto d3d12Device = device->GetDevice();

		D3D12_HEAP_PROPERTIES uploadProperties;
		uploadProperties.Type				  = D3D12_HEAP_TYPE_UPLOAD;
		uploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadProperties.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadProperties.CreationNodeMask	  = 0;
		uploadProperties.VisibleNodeMask	  = 0;

		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment			= D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		uploadBufferDesc.Width				= m_Description.Size;
		uploadBufferDesc.Height				= 1;
		uploadBufferDesc.DepthOrArraySize	= 1;
		uploadBufferDesc.MipLevels			= 1;
		uploadBufferDesc.Format				= DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count	= 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags				= D3D12_RESOURCE_FLAG_NONE;
		d3d12Device->CreateCommittedResource(&uploadProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &uploadBufferDesc,
											 D3D12_RESOURCE_STATE_COMMON,
											 nullptr,
											 IID_PPV_ARGS(&m_UploadBuffer));

		D3D12_HEAP_PROPERTIES bufferProperties;
		bufferProperties.Type				  = D3D12_HEAP_TYPE_DEFAULT;
		bufferProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		bufferProperties.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		bufferProperties.CreationNodeMask	  = 0;
		bufferProperties.VisibleNodeMask	  = 0;

		D3D12_RESOURCE_DESC bufferDesc;
		bufferDesc.Dimension		  = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment		  = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		bufferDesc.Width			  = m_Description.Size;
		bufferDesc.Height			  = 1;
		bufferDesc.DepthOrArraySize	  = 1;
		bufferDesc.MipLevels		  = 1;
		bufferDesc.Format			  = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count	  = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout			  = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags			  = D3D12_RESOURCE_FLAG_NONE;
		d3d12Device->CreateCommittedResource(&bufferProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &bufferDesc,
											 D3D12_RESOURCE_STATE_COMMON,
											 nullptr,
											 IID_PPV_ARGS(&m_IndexBuffer));

		m_UploadRange.Begin = 0;
		m_UploadRange.End	= m_Description.Size;

		CreateIndexBufferFormat();
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes	 = m_Description.Size;
		m_IndexBufferView.Format		 = m_IndexBufferFormat;

		if (!data)
			return;

		SetData(data, m_Description.Size, 0);
	}

	IndexBufferD3D12::~IndexBufferD3D12()
	{
	}

	void IndexBufferD3D12::CreateIndexBufferFormat()
	{
		switch (m_Format)
		{
			case Nexus::Graphics::IndexBufferFormat::UInt16: m_IndexBufferFormat = DXGI_FORMAT_R16_UINT; break;
			case Nexus::Graphics::IndexBufferFormat::UInt32: m_IndexBufferFormat = DXGI_FORMAT_R32_UINT; break;
		}
	}

	const D3D12_INDEX_BUFFER_VIEW IndexBufferD3D12::GetIndexBufferView() const
	{
		return m_IndexBufferView;
	}

	void IndexBufferD3D12::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		void *buffer;
		m_UploadBuffer->Map(0, &m_UploadRange, &buffer);

		void *offsetIntoBuffer = (void *)(((const char *)buffer) + offset);
		memcpy((void *)offsetIntoBuffer, data, size);

		m_UploadBuffer->Unmap(0, &m_UploadRange);
		m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd)
								  { cmd->CopyBufferRegion(m_IndexBuffer.Get(), 0, m_UploadBuffer.Get(), 0, m_Description.Size); });
	}

	UniformBufferD3D12::UniformBufferD3D12(GraphicsDeviceD3D12 *device, const BufferDescription &description, const void *data)
		: UniformBuffer(description, data),
		  m_Device(device)
	{
		auto d3d12Device = device->GetDevice();

		D3D12_HEAP_PROPERTIES uploadProperties;
		uploadProperties.Type				  = D3D12_HEAP_TYPE_UPLOAD;
		uploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadProperties.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadProperties.CreationNodeMask	  = 0;
		uploadProperties.VisibleNodeMask	  = 0;

		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

		// convert the size of the constant buffer to be 256 byte aligned
		uploadBufferDesc.Width				= (m_Description.Size + 255) & ~255;
		uploadBufferDesc.Height				= 1;
		uploadBufferDesc.DepthOrArraySize	= 1;
		uploadBufferDesc.MipLevels			= 1;
		uploadBufferDesc.Format				= DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count	= 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags				= D3D12_RESOURCE_FLAG_NONE;

		d3d12Device->CreateCommittedResource(&uploadProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &uploadBufferDesc,
											 D3D12_RESOURCE_STATE_COMMON,
											 nullptr,
											 IID_PPV_ARGS(&m_UploadBuffer));

		D3D12_HEAP_PROPERTIES bufferProperties;
		bufferProperties.Type				  = D3D12_HEAP_TYPE_DEFAULT;
		bufferProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		bufferProperties.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		bufferProperties.CreationNodeMask	  = 0;
		bufferProperties.VisibleNodeMask	  = 0;

		D3D12_RESOURCE_DESC bufferDesc;
		bufferDesc.Dimension		  = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment		  = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		bufferDesc.Width			  = (m_Description.Size + 255) & ~255;
		bufferDesc.Height			  = 1;
		bufferDesc.DepthOrArraySize	  = 1;
		bufferDesc.MipLevels		  = 1;
		bufferDesc.Format			  = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count	  = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout			  = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags			  = D3D12_RESOURCE_FLAG_NONE;

		d3d12Device->CreateCommittedResource(&bufferProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &bufferDesc,
											 D3D12_RESOURCE_STATE_COMMON,
											 nullptr,
											 IID_PPV_ARGS(&m_ConstantBuffer));

		m_UploadRange.Begin = 0;
		m_UploadRange.End	= m_Description.Size;

		if (!data)
			return;

		SetData(data, m_Description.Size, 0);
	}

	UniformBufferD3D12::~UniformBufferD3D12()
	{
	}

	ID3D12Resource2 *UniformBufferD3D12::GetHandle()
	{
		return m_ConstantBuffer.Get();
	}

	void UniformBufferD3D12::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		void *buffer;
		m_UploadBuffer->Map(0, &m_UploadRange, &buffer);

		void *offsetIntoBuffer = (void *)(((const char *)buffer) + offset);
		memcpy((void *)offsetIntoBuffer, data, size);

		m_UploadBuffer->Unmap(0, &m_UploadRange);
		m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd)
								  { cmd->CopyBufferRegion(m_ConstantBuffer.Get(), 0, m_UploadBuffer.Get(), 0, m_Description.Size); });
	}
}	 // namespace Nexus::Graphics
#endif