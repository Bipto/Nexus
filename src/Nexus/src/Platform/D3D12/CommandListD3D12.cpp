#include "CommandListD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "DeviceBufferD3D12.hpp"
	#include "D3D12Utils.hpp"
	#include "FramebufferD3D12.hpp"
	#include "PipelineD3D12.hpp"
	#include "ResourceSetD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TimingQueryD3D12.hpp"

namespace Nexus::Graphics
{
	CommandListD3D12::CommandListD3D12(GraphicsDeviceD3D12 *device, const CommandListSpecification &spec) : CommandList(spec)
	{
		auto d3d12Device = device->GetDevice();
		d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
		d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CommandList));
	}

	CommandListD3D12::~CommandListD3D12()
	{
	}

	ID3D12GraphicsCommandList7 *CommandListD3D12::GetCommandList()
	{
		return m_CommandList.Get();
	}

	void CommandListD3D12::Reset()
	{
		m_CommandAllocator->Reset();
		m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
	}

	void CommandListD3D12::Close()
	{
		m_CommandList->Close();
	}
}	 // namespace Nexus::Graphics

#endif