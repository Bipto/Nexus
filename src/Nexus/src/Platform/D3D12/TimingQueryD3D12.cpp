#if defined(NX_PLATFORM_D3D12)

	#include "TimingQueryD3D12.hpp"

namespace Nexus::Graphics
{
	TimingQueryD3D12::TimingQueryD3D12(GraphicsDeviceD3D12 *device) : m_Device(device)
	{
		Microsoft::WRL::ComPtr<ID3D12Device9> d3d12Device = m_Device->GetD3D12Device();

		CreateQueryHeap();
		CreateReadbackBuffer();
	}

	TimingQueryD3D12::~TimingQueryD3D12()
	{
	}

	void TimingQueryD3D12::Resolve()
	{
		m_Device->ImmediateSubmit([&](ID3D12GraphicsCommandList7 *cmd)
								  { cmd->ResolveQueryData(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, 2, m_ReadbackBuffer.Get(), 0); });

		uint64_t *timestamps;
		m_ReadbackBuffer->Map(0, nullptr, reinterpret_cast<void **>(&timestamps));
		uint64_t start = timestamps[0];
		uint64_t end   = timestamps[1];
		m_ReadbackBuffer->Unmap(0, nullptr);

		uint64_t								   frequency;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue = m_Device->GetCommandQueue();
		queue->GetTimestampFrequency(&frequency);
		m_ElapsedTime = (end - start) / 1000000.0f;
	}

	float TimingQueryD3D12::GetElapsedMilliseconds()
	{
		return m_ElapsedTime;
	}

	Microsoft::WRL::ComPtr<ID3D12QueryHeap> TimingQueryD3D12::GetQueryHeap()
	{
		return m_QueryHeap;
	}

	void TimingQueryD3D12::CreateQueryHeap()
	{
		Microsoft::WRL::ComPtr<ID3D12Device9> d3d12Device = m_Device->GetD3D12Device();

		// create query heap
		D3D12_QUERY_HEAP_DESC queryHeapDesc = {};
		queryHeapDesc.Type					= D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		queryHeapDesc.Count					= 2;
		queryHeapDesc.NodeMask				= 0;
		d3d12Device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_QueryHeap));
	}

	void TimingQueryD3D12::CreateReadbackBuffer()
	{
		Microsoft::WRL::ComPtr<ID3D12Device9> d3d12Device = m_Device->GetD3D12Device();

		// create the heap
		D3D12_HEAP_PROPERTIES readbackHeapProperties = {};
		readbackHeapProperties.Type					 = D3D12_HEAP_TYPE_READBACK;
		readbackHeapProperties.CPUPageProperty		 = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		readbackHeapProperties.MemoryPoolPreference	 = D3D12_MEMORY_POOL_UNKNOWN;
		readbackHeapProperties.CreationNodeMask		 = 1;
		readbackHeapProperties.VisibleNodeMask		 = 1;

		// create the buffer
		D3D12_RESOURCE_DESC readbackBufferDesc = {};
		readbackBufferDesc.Dimension		   = D3D12_RESOURCE_DIMENSION_BUFFER;
		readbackBufferDesc.Alignment		   = 0;
		readbackBufferDesc.Width			   = sizeof(uint64_t) * 2;
		readbackBufferDesc.Height			   = 1;
		readbackBufferDesc.DepthOrArraySize	   = 1;
		readbackBufferDesc.MipLevels		   = 1;
		readbackBufferDesc.Format			   = DXGI_FORMAT_UNKNOWN;
		readbackBufferDesc.SampleDesc.Count	   = 1;
		readbackBufferDesc.SampleDesc.Quality  = 0;
		readbackBufferDesc.Layout			   = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		readbackBufferDesc.Flags			   = D3D12_RESOURCE_FLAG_NONE;

		d3d12Device->CreateCommittedResource(&readbackHeapProperties,
											 D3D12_HEAP_FLAG_NONE,
											 &readbackBufferDesc,
											 D3D12_RESOURCE_STATE_COPY_DEST,
											 nullptr,
											 IID_PPV_ARGS(&m_ReadbackBuffer));
	}
}	 // namespace Nexus::Graphics

#endif