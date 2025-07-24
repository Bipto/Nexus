#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

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
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), physicalDeviceD3D12->GetMaximumSupportedFeatureLevel(), IID_PPV_ARGS(&m_Device))))
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

		GetLimitsAndFeatures();
	}

	GraphicsDeviceD3D12::~GraphicsDeviceD3D12()
	{
		if (m_FenceEvent)
		{
			CloseHandle(m_FenceEvent);
		}
	}

	void GraphicsDeviceD3D12::SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence)
	{
		Ref<CommandListD3D12>								   commandListD3D12 = std::dynamic_pointer_cast<CommandListD3D12>(commandList);
		const std::vector<Nexus::Graphics::RenderCommandData> &commands			= commandListD3D12->GetCommandData();
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7>	   cmdList			= commandListD3D12->GetCommandList();

		commandListD3D12->Reset();
		m_CommandExecutor->SetCommandList(cmdList);
		m_CommandExecutor->ExecuteCommands(commands, this);
		commandListD3D12->Close();
		m_CommandExecutor->Reset();

		ID3D12CommandList *lists[] = {cmdList.Get()};
		m_CommandQueue->ExecuteCommandLists(1, lists);

		if (fence)
		{
			Ref<FenceD3D12>						 fenceD3D12	 = std::dynamic_pointer_cast<FenceD3D12>(fence);
			Microsoft::WRL::ComPtr<ID3D12Fence1> fenceHandle = fenceD3D12->GetHandle();
			m_CommandQueue->Signal(fenceHandle.Get(), 1);
			NX_ASSERT(fenceHandle->SetEventOnCompletion(1, fenceD3D12->GetFenceEvent()), "Failed to set event on completion");
		}
	}

	void GraphicsDeviceD3D12::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
	{
		std::vector<ID3D12CommandList *> d3d12CommandLists(numCommandLists);

		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListD3D12>								   commandList = std::dynamic_pointer_cast<CommandListD3D12>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7>	   cmdList	   = commandList->GetCommandList();

			commandList->Reset();
			m_CommandExecutor->SetCommandList(cmdList);
			m_CommandExecutor->ExecuteCommands(commands, this);
			commandList->Close();
			m_CommandExecutor->Reset();

			d3d12CommandLists[i] = cmdList.Get();
		}

		m_CommandQueue->ExecuteCommandLists(d3d12CommandLists.size(), d3d12CommandLists.data());

		if (fence)
		{
			Ref<FenceD3D12> fenceD3D12	= std::dynamic_pointer_cast<FenceD3D12>(fence);
			Microsoft::WRL::ComPtr<ID3D12Fence1> fenceHandle = fenceD3D12->GetHandle();
			m_CommandQueue->Signal(fenceHandle.Get(), 1);
			NX_ASSERT(fenceHandle->SetEventOnCompletion(1, fenceD3D12->GetFenceEvent()), "Failed to set event on completion");
		}
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

	Ref<CommandList> GraphicsDeviceD3D12::CreateCommandList(const CommandListDescription &spec)
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

	Ref<Sampler> GraphicsDeviceD3D12::CreateSampler(const SamplerDescription &spec)
	{
		return CreateRef<SamplerD3D12>(spec);
	}

	Ref<TimingQuery> GraphicsDeviceD3D12::CreateTimingQuery()
	{
		return CreateRef<TimingQueryD3D12>(this);
	}

	Ref<DeviceBuffer> GraphicsDeviceD3D12::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return CreateRef<DeviceBufferD3D12>(desc, this);
	}

	Microsoft::WRL::ComPtr<D3D12MA::Allocator> GraphicsDeviceD3D12::GetAllocator()
	{
		return m_Allocator.Get();
	}

	Microsoft::WRL::ComPtr<IDXGIFactory7> GraphicsDeviceD3D12::GetDXGIFactory() const
	{
		return m_DxgiFactory.Get();
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GraphicsDeviceD3D12::GetCommandQueue() const
	{
		return m_CommandQueue.Get();
	}

	bool GraphicsDeviceD3D12::IsBufferUsageSupported(BufferUsage usage)
	{
		return false;
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

	Ref<Texture> GraphicsDeviceD3D12::CreateTexture(const TextureDescription &spec)
	{
		return CreateRef<TextureD3D12>(spec, this);
	}

	Ref<Swapchain> GraphicsDeviceD3D12::CreateSwapchain(IWindow *window, const SwapchainSpecification &spec)
	{
		return CreateRef<SwapchainD3D12>(window, this, spec);
	}

	Ref<Fence> GraphicsDeviceD3D12::CreateFence(const FenceDescription &desc)
	{
		return CreateRef<FenceD3D12>(desc, this);
	}

	FenceWaitResult GraphicsDeviceD3D12::WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout)
	{
		std::vector<HANDLE> eventHandles(count);
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceD3D12> fence = std::dynamic_pointer_cast<FenceD3D12>(fences[i]);
			eventHandles[i]		  = fence->GetFenceEvent();
		}

		DWORD result = WaitForMultipleObjects(eventHandles.size(), eventHandles.data(), waitAll, timeout.GetMilliseconds<uint64_t>());

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

	void GraphicsDeviceD3D12::ResetFences(Ref<Fence> *fences, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceD3D12> fence = std::dynamic_pointer_cast<FenceD3D12>(fences[i]);
			fence->Reset();
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Device9> GraphicsDeviceD3D12::GetDevice() const
	{
		return m_Device;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GraphicsDeviceD3D12::GetUploadCommandList()
	{
		return m_UploadCommandList;
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
		ResourceBarrier(cmd, resource->GetHandle().Get(), layer, level, resource->GetDescription().MipLevels, resourceState, after);
		resource->SetResourceState(layer, level, after);
	}

	void GraphicsDeviceD3D12::ResourceBarrierBuffer(ID3D12GraphicsCommandList7 *cmd,
													Ref<DeviceBufferD3D12>		buffer,
													D3D12_RESOURCE_STATES		before,
													D3D12_RESOURCE_STATES		after)
	{
		auto resourceHandle = buffer->GetHandle();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = resourceHandle.Get();
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter  = after;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		cmd->ResourceBarrier(1, &barrier);
	}

	void GraphicsDeviceD3D12::ResourceBarrierSwapchainColour(ID3D12GraphicsCommandList7 *cmd,
															 Ref<SwapchainD3D12>		 resource,
															 D3D12_RESOURCE_STATES		 after)
	{
		ResourceBarrier(cmd, resource->RetrieveBufferHandle().Get(), 0, 0, 1, resource->GetCurrentTextureState(), after);
		resource->SetTextureState(after);
	}

	void GraphicsDeviceD3D12::ResourceBarrierSwapchainDepth(ID3D12GraphicsCommandList7 *cmd,
															Ref<SwapchainD3D12>			resource,
															D3D12_RESOURCE_STATES		after)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource2> handle = resource->RetrieveDepthBufferHandle();
		ResourceBarrier(cmd, handle.Get(), 0, 0, 1, resource->GetCurrentDepthState(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
		resource->SetDepthState(after);
	}

	bool GraphicsDeviceD3D12::Validate()
	{
		return m_Device && m_CommandQueue && m_Fence && m_UploadCommandAllocator && m_UploadCommandList && m_DxgiFactory;
	}

	PixelFormatProperties GraphicsDeviceD3D12::GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const
	{
		PixelFormatProperties properties = {};
		return properties;
	}

	const DeviceFeatures &GraphicsDeviceD3D12::GetPhysicalDeviceFeatures() const
	{
		return m_Features;
	}

	const DeviceLimits &GraphicsDeviceD3D12::GetPhysicalDeviceLimits() const
	{
		return m_Limits;
	}

	bool GraphicsDeviceD3D12::IsIndexBufferFormatSupported(IndexFormat format) const
	{
		switch (format)
		{
			case IndexFormat::UInt8: return false;
			case IndexFormat::UInt16:
			case IndexFormat::UInt32: return true;
			default: throw std::runtime_error("Failed to find index buffer format");
		}
	}

	AccelerationStructureBuildSizeDescription GraphicsDeviceD3D12::GetAccelerationStructureBuildSize(
		const AccelerationStructureBuildDescription &description,
		const std::vector<uint32_t>					&primitiveCount) const
	{
		return AccelerationStructureBuildSizeDescription();
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

	void GraphicsDeviceD3D12::GetLimitsAndFeatures()
	{
		D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT vaSupport = {};

		m_Limits.Texture1D = true;
		m_Limits.Texture2D = true;
		m_Limits.Texture3D = true;

		m_Limits.MaxUniformBufferRange = D3D12_REQ_CONSTANT_BUFFER_ELEMENT_COUNT;

		HRESULT hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &vaSupport, sizeof(vaSupport));
		if (SUCCEEDED(hr))
		{
			m_Limits.MaxStorageBufferRange = vaSupport.MaxGPUVirtualAddressBitsPerResource;
		}

		m_Limits.MaxVertexInputAttributes			 = 32;
		m_Limits.MaxVertexInputBindings				 = 32;
		m_Limits.MaxVertexInputOffset				 = std::numeric_limits<uint32_t>::max();
		m_Limits.MaxVertexInputStride				 = 255;
		m_Limits.MaxFramebufferColourAttachmentCount = 8;
		m_Limits.MaxViewports						 = D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

		m_Features.SupportsGeometryShaders				= true;
		m_Features.SupportsTesselationShaders			= true;
		m_Features.SupportsComputeShaders				= true;
		m_Features.SupportsStorageBuffers				= true;
		m_Features.SupportsMultiviewport				= true;
		m_Features.SupportsSamplerAnisotropy			= true;
		m_Features.SupportsETC2Compression				= false;
		m_Features.SupportsASTC_LDRCompression			= true;
		m_Features.SupportsBCCompression				= true;
		m_Features.SupportShaderStorageImageMultisample = true;
		m_Features.SupportsCubemapArray					= true;
		m_Features.SupportsIndependentBlend				= true;

		m_Features.SupportsMeshTaskShaders = true;
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
	}	 // namespace Nexus::Graphics
}	 // namespace Nexus::Graphics
#endif