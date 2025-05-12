#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "DeviceBufferD3D12.hpp"
	#include "CommandListD3D12.hpp"
	#include "FramebufferD3D12.hpp"
	#include "PipelineD3D12.hpp"
	#include "ResourceSetD3D12.hpp"
	#include "SamplerD3D12.hpp"
	#include "ShaderModuleD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TimingQueryD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "PhysicalDeviceD3D12.hpp"
	#include "DeviceBufferD3D12.hpp"
	#include "FenceD3D12.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceD3D12::GraphicsDeviceD3D12(std::shared_ptr<IPhysicalDevice> physicalDevice, Microsoft::WRL::ComPtr<IDXGIFactory7> factory)
		: m_DxgiFactory(factory),
		  m_PhysicalDevice(physicalDevice)
	{
		std::shared_ptr<PhysicalDeviceD3D12>  physicalDeviceD3D12 = std::dynamic_pointer_cast<PhysicalDeviceD3D12>(physicalDevice);
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter			  = physicalDeviceD3D12->GetAdapter();

		// create the D3D12Device
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device))))
		{
			// Create a command queue to submit work to the GPU
			D3D12_COMMAND_QUEUE_DESC commandQueueDesc {};
			commandQueueDesc.Type	  = D3D12_COMMAND_LIST_TYPE_DIRECT;
			commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
			commandQueueDesc.NodeMask = 0;
			commandQueueDesc.Flags	  = D3D12_COMMAND_QUEUE_FLAG_NONE;

			// create the command queue
			if (SUCCEEDED(m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue))))
			{
				// create the fence
				if (SUCCEEDED(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))))
				{
					m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
				}

				m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_UploadCommandAllocator));
				m_Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_UploadCommandList));
			}

			D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
			allocatorDesc.pDevice				  = m_Device.Get();
			allocatorDesc.pAdapter				  = adapter.Get();
			HRESULT hr							  = D3D12MA::CreateAllocator(&allocatorDesc, &m_Allocator);
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to create allocator");
			}
		}

		m_CommandExecutor = std::make_unique<CommandExecutorD3D12>(m_Device);
	}

	GraphicsDeviceD3D12::~GraphicsDeviceD3D12()
	{
		if (m_FenceEvent)
		{
			CloseHandle(m_FenceEvent);
		}
	}

	void GraphicsDeviceD3D12::SubmitCommandList(Ref<CommandList> commandList)
	{
		Ref<CommandListD3D12> d3d12CommandList = std::dynamic_pointer_cast<CommandListD3D12>(commandList);

		const std::vector<Nexus::Graphics::RenderCommandData> &commands = d3d12CommandList->GetCommandData();
		ID3D12GraphicsCommandList7							  *cmdList	= d3d12CommandList->GetCommandList();

		d3d12CommandList->Reset();
		m_CommandExecutor->SetCommandList(cmdList);
		m_CommandExecutor->ExecuteCommands(commands, this);
		d3d12CommandList->Close();
		m_CommandExecutor->Reset();

		ID3D12CommandList *lists[] = {cmdList};
		m_CommandQueue->ExecuteCommandLists(1, lists);

		SignalAndWait();
	}

	const std::string GraphicsDeviceD3D12::GetAPIName()
	{
		return {"D3D12"};
	}

	const char *GraphicsDeviceD3D12::GetDeviceName()
	{
		return m_PhysicalDevice->GetDeviceName().c_str();
	}

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceD3D12::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	Ref<ShaderModule> GraphicsDeviceD3D12::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
	{
		return CreateRef<ShaderModuleD3D12>(moduleSpec, resources);
	}

	Ref<GraphicsPipeline> GraphicsDeviceD3D12::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineD3D12>(m_Device.Get(), description);
	}

	Ref<ComputePipeline> GraphicsDeviceD3D12::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		return CreateRef<ComputePipelineD3D12>(m_Device.Get(), description);
	}

	Ref<CommandList> GraphicsDeviceD3D12::CreateCommandList(const CommandListSpecification &spec)
	{
		return CreateRef<CommandListD3D12>(this, spec);
	}

	Ref<ResourceSet> GraphicsDeviceD3D12::CreateResourceSet(const ResourceSetSpecification &spec)
	{
		return CreateRef<ResourceSetD3D12>(spec, this);
	}

	Ref<Framebuffer> GraphicsDeviceD3D12::CreateFramebuffer(const FramebufferSpecification &spec)
	{
		return CreateRef<FramebufferD3D12>(spec, this);
	}

	Ref<Sampler> GraphicsDeviceD3D12::CreateSampler(const SamplerSpecification &spec)
	{
		return CreateRef<SamplerD3D12>(spec);
	}

	Ref<TimingQuery> GraphicsDeviceD3D12::CreateTimingQuery()
	{
		return CreateRef<TimingQueryD3D12>(this);
	}

	DeviceBuffer *GraphicsDeviceD3D12::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return new DeviceBufferD3D12(desc, this);
	}

	D3D12MA::Allocator *GraphicsDeviceD3D12::GetAllocator()
	{
		return m_Allocator.Get();
	}

	IDXGIFactory7 *GraphicsDeviceD3D12::GetDXGIFactory() const
	{
		return m_DxgiFactory.Get();
	}

	ID3D12CommandQueue *GraphicsDeviceD3D12::GetCommandQueue() const
	{
		return m_CommandQueue.Get();
	}

	void GraphicsDeviceD3D12::WaitForIdle()
	{
		SignalAndWait();
	}

	GraphicsAPI GraphicsDeviceD3D12::GetGraphicsAPI()
	{
		return GraphicsAPI::D3D12;
	}

	const GraphicsCapabilities GraphicsDeviceD3D12::GetGraphicsCapabilities() const
	{
		GraphicsCapabilities capabilities;
		capabilities.SupportsMultisampledTextures		 = true;
		capabilities.SupportsLODBias					 = true;
		capabilities.SupportsInstanceOffset				 = true;
		capabilities.SupportsMultipleSwapchains			 = true;
		capabilities.SupportsSeparateColourAndBlendMasks = true;
		return capabilities;
	}

	Texture *GraphicsDeviceD3D12::CreateTexture(const TextureSpecification &spec)
	{
		return new TextureD3D12(spec, this);
	}

	Swapchain *GraphicsDeviceD3D12::CreateSwapchain(IWindow *window, const SwapchainSpecification &spec)
	{
		return new SwapchainD3D12(window, this, spec);
	}

	Fence *GraphicsDeviceD3D12::CreateFence(const FenceDescription &desc)
	{
		return new FenceD3D12(desc, this);
	}

	FenceWaitResult GraphicsDeviceD3D12::WaitForFences(Fence **fences, uint32_t count, bool waitAll, TimeSpan timeout)
	{
		std::vector<HANDLE> eventHandles(count);
		for (uint32_t i = 0; i < count; i++)
		{
			FenceD3D12 *fence = (FenceD3D12 *)fences[i];
			eventHandles[i]	  = fence->GetFenceEvent();
		}

		DWORD result = WaitForMultipleObjects(eventHandles.size(), eventHandles.data(), waitAll, timeout.GetMilliseconds());

		if (result == WAIT_OBJECT_0)
		{
			return FenceWaitResult::Signalled;
		}
		else if (result == WAIT_TIMEOUT)
		{
			return FenceWaitResult::TimedOut;
		}
		else
		{
			return FenceWaitResult::Failed;
		}
	}

	void GraphicsDeviceD3D12::ResetFences(Fence **fences, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			FenceD3D12 *fence = (FenceD3D12 *)fences[i];
			fence->Reset();
		}
	}

	ID3D12Device9 *GraphicsDeviceD3D12::GetDevice() const
	{
		return m_Device.Get();
	}

	ID3D12GraphicsCommandList7 *GraphicsDeviceD3D12::GetUploadCommandList()
	{
		return m_UploadCommandList.Get();
	}

	void GraphicsDeviceD3D12::SignalAndWait()
	{
		m_CommandQueue->Signal(m_Fence.Get(), ++m_FenceValue);

		if (SUCCEEDED(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent)))
		{
			if (WaitForSingleObject(m_FenceEvent, 20000) != WAIT_OBJECT_0)
			{
				throw std::runtime_error("Failed to wait for fence event");
			}
		}
		else
		{
			throw std::runtime_error("Failed to set fence event");
		}
	}

	void GraphicsDeviceD3D12::ImmediateSubmit(std::function<void(ID3D12GraphicsCommandList7 *cmd)> &&function)
	{
		InitUploadCommandList();
		function(m_UploadCommandList.Get());
		DispatchUploadCommandList();
	}

	void GraphicsDeviceD3D12::ResourceBarrier(ID3D12GraphicsCommandList7 *cmd,
											  ID3D12Resource			 *resource,
											  uint32_t					  layer,
											  uint32_t					  level,
											  uint32_t					  mipCount,
											  D3D12_RESOURCE_STATES		  before,
											  D3D12_RESOURCE_STATES		  after)
	{
		if (before == after)
		{
			return;
		}

		uint32_t subresourceIndex = Utils::CalculateSubresource(layer, level, mipCount);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = resource;
		barrier.Transition.Subresource = subresourceIndex;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter  = after;
		cmd->ResourceBarrier(1, &barrier);
	}

	void GraphicsDeviceD3D12::ResourceBarrier(ID3D12GraphicsCommandList7 *cmd,
											  Ref<TextureD3D12>			  resource,
											  uint32_t					  layer,
											  uint32_t					  level,
											  D3D12_RESOURCE_STATES		  after)
	{
		D3D12_RESOURCE_STATES resourceState = resource->GetResourceState(layer, level);
		ResourceBarrier(cmd, resource->GetHandle().Get(), layer, level, resource->GetSpecification().MipLevels, resourceState, after);
		resource->SetResourceState(layer, level, after);
	}

	void GraphicsDeviceD3D12::ResourceBarrierSwapchainColour(ID3D12GraphicsCommandList7 *cmd, SwapchainD3D12 *resource, D3D12_RESOURCE_STATES after)
	{
		ResourceBarrier(cmd, resource->RetrieveBufferHandle().Get(), 0, 0, 1, resource->GetCurrentTextureState(), after);
		resource->SetTextureState(after);
	}

	void GraphicsDeviceD3D12::ResourceBarrierSwapchainDepth(ID3D12GraphicsCommandList7 *cmd, SwapchainD3D12 *resource, D3D12_RESOURCE_STATES after)
	{
		ResourceBarrier(cmd, resource->RetrieveDepthBufferHandle(), 0, 0, 1, resource->GetCurrentDepthState(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
		resource->SetDepthState(after);
	}

	bool GraphicsDeviceD3D12::Validate()
	{
		return m_Device && m_CommandQueue && m_Fence && m_UploadCommandAllocator && m_UploadCommandList && m_DxgiFactory;
	}

	void GraphicsDeviceD3D12::SetName(const std::string &name)
	{
		GraphicsDevice::SetName(name);
	}

	void GraphicsDeviceD3D12::InitUploadCommandList()
	{
		m_UploadCommandAllocator->Reset();
		m_UploadCommandList->Reset(m_UploadCommandAllocator.Get(), nullptr);
	}

	void GraphicsDeviceD3D12::DispatchUploadCommandList()
	{
		if (SUCCEEDED(m_UploadCommandList->Close()))
		{
			ID3D12CommandList *list[] = {m_UploadCommandList.Get()};
			m_CommandQueue->ExecuteCommandLists(1, list);
			SignalAndWait();
		}
	}

	inline void GraphicsDeviceD3D12::ReportLiveObjects()
	{
		// initialise dxgi debug layer
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			OutputDebugStringW(L"Reporting live D3D12 objects:\n");
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
}	 // namespace Nexus::Graphics

#endif