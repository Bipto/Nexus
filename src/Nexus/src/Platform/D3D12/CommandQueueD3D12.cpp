#include "CommandQueueD3D12.hpp"
#include "CommandListD3D12.hpp"
#include "FenceD3D12.hpp"

namespace Nexus::Graphics
{
	CommandQueueD3D12::CommandQueueD3D12(GraphicsDeviceD3D12 *device, const CommandQueueDescription &description)
		: m_Device(device),
		  m_Description(description)
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type					  = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority				  = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDesc.NodeMask				  = 0;
		commandQueueDesc.Flags					  = D3D12_COMMAND_QUEUE_FLAG_NONE;

		auto d3d12Device = m_Device->GetD3D12Device();
		d3d12Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue));

		std::wstring name = {m_Description.DebugName.begin(), m_Description.DebugName.end()};
		m_CommandQueue->SetName(name.c_str());

		m_CommandExecutor = std::make_unique<CommandExecutorD3D12>(m_Device->GetD3D12Device());

		Graphics::FenceDescription fenceDesc = {};
		fenceDesc.Signalled					 = false;
		m_Fence								 = device->CreateFence(fenceDesc);
	}

	CommandQueueD3D12::~CommandQueueD3D12()
	{
	}

	void CommandQueueD3D12::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
	{
		/*std::vector<ID3D12CommandList *> d3d12CommandLists(numCommandLists);

		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListD3D12>								   commandList = std::dynamic_pointer_cast<CommandListD3D12>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7>	   cmdList	   = commandList->GetCommandList();

			commandList->Reset();
			m_CommandExecutor->SetCommandList(cmdList);
			m_CommandExecutor->ExecuteCommands(commandList, m_Device);
			commandList->Close();
			m_CommandExecutor->Reset();

			d3d12CommandLists[i] = cmdList.Get();
		}

		m_CommandQueue->ExecuteCommandLists(d3d12CommandLists.size(), d3d12CommandLists.data());

		if (fence)
		{
			Ref<FenceD3D12>						 fenceD3D12	 = std::dynamic_pointer_cast<FenceD3D12>(fence);
			Microsoft::WRL::ComPtr<ID3D12Fence1> fenceHandle = fenceD3D12->GetHandle();
			m_CommandQueue->Signal(fenceHandle.Get(), 1);
			NX_VALIDATE(SUCCEEDED(fenceHandle->SetEventOnCompletion(1, fenceD3D12->GetFenceEvent())), "Failed to set event on completion");
		}*/

		assert(0);
	}

	void CommandQueueD3D12::Present(Ref<Swapchain> swapchain)
	{
		Ref<SwapchainD3D12> swapchainD3D12 = std::dynamic_pointer_cast<SwapchainD3D12>(swapchain);
		swapchainD3D12->SwapBuffers();
	}

	GraphicsDevice *CommandQueueD3D12::GetGraphicsDevice()
	{
		return m_Device;
	}

	bool CommandQueueD3D12::WaitForIdle()
	{
		m_Device->WaitForQueueIdle(m_CommandQueue);
		return true;
	}
}	 // namespace Nexus::Graphics
