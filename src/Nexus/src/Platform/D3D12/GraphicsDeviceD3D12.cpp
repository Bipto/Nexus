#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "CommandListD3D12.hpp"
	#include "CommandQueueD3D12.hpp"
	#include "DeviceBufferD3D12.hpp"
	#include "FenceD3D12.hpp"
	#include "FramebufferD3D12.hpp"
	#include "PhysicalDeviceD3D12.hpp"
	#include "PipelineD3D12.hpp"
	#include "ResourceSetD3D12.hpp"
	#include "SamplerD3D12.hpp"
	#include "ShaderModuleD3D12.hpp"
	#include "SwapchainD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TimingQueryD3D12.hpp"

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
			// NX_VALIDATE(SUCCEEDED(baseDevice->QueryInterface(IID_PPV_ARGS(&m_Device))), "Failed to get interface");

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

	const std::string GraphicsDeviceD3D12::GetAPIName()
	{
		return {"D3D12"};
	}

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceD3D12::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	Ref<ShaderModule> GraphicsDeviceD3D12::CreateShaderModule(const ShaderModuleSpecification &moduleSpec)
	{
		return CreateRef<ShaderModuleD3D12>(moduleSpec);
	}

	Ref<GraphicsPipeline> GraphicsDeviceD3D12::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineD3D12>(this, description);
	}

	Ref<ComputePipeline> GraphicsDeviceD3D12::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		return CreateRef<ComputePipelineD3D12>(this, description);
	}

	Ref<MeshletPipeline> GraphicsDeviceD3D12::CreateMeshletPipeline(const MeshletPipelineDescription &description)
	{
		return CreateRef<MeshletPipelineD3D12>(this, description);
	}

	Ref<CommandList> GraphicsDeviceD3D12::CreateCommandList(const CommandListDescription &spec)
	{
		return CreateRef<CommandListD3D12>(this, spec);
	}

	Ref<RayTracingPipeline> GraphicsDeviceD3D12::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
	{
		return Ref<RayTracingPipeline>();
	}

	Ref<ResourceSet> GraphicsDeviceD3D12::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		return CreateRef<ResourceSetD3D12>(pipeline, this);
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

	Ref<IAccelerationStructure> GraphicsDeviceD3D12::CreateAccelerationStructure(const AccelerationStructureDescription &desc)
	{
		return Ref<IAccelerationStructure>();
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

	Microsoft::WRL::ComPtr<ID3D12Device9> GraphicsDeviceD3D12::GetD3D12Device() const
	{
		return m_Device.Get();
	}

	bool GraphicsDeviceD3D12::IsBufferUsageSupported(BufferUsage usage)
	{
		return false;
	}

	void GraphicsDeviceD3D12::WaitForIdle()
	{
		SignalAndWait(m_CommandQueue);
	}

	void GraphicsDeviceD3D12::WaitForQueueIdle(Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue)
	{
		SignalAndWait(queue);
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

	Ref<Swapchain> GraphicsDeviceD3D12::CreateSwapchain(IWindow *window, const SwapchainDescription &spec)
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

	std::vector<QueueFamilyInfo> GraphicsDeviceD3D12::GetQueueFamilies()
	{
		std::vector<QueueFamilyInfo> queueFamilies = {};

		QueueFamilyInfo &info = queueFamilies.emplace_back();
		info.QueueFamily	  = 0;
		info.QueueCount		  = std::numeric_limits<uint32_t>::max();
		info.Capabilities	  = QueueCapabilities(QueueCapabilities::Graphics | QueueCapabilities::Compute | QueueCapabilities::Transfer);

		return queueFamilies;
	}

	Ref<ICommandQueue> GraphicsDeviceD3D12::CreateCommandQueue(const CommandQueueDescription &description)
	{
		Ref<ICommandQueue>	   commandQueue		 = CreateRef<CommandQueueD3D12>(this, description);
		Ref<CommandQueueD3D12> commandQueueD3D12 = std::dynamic_pointer_cast<CommandQueueD3D12>(commandQueue);

		WeakRef<CommandQueueD3D12> commandQueueWeakRef = commandQueueD3D12;
		m_CreatedCommandQueues.push_back(commandQueueWeakRef);

		return commandQueue;
	}

	void GraphicsDeviceD3D12::ResetFences(Ref<Fence> *fences, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceD3D12> fence = std::dynamic_pointer_cast<FenceD3D12>(fences[i]);
			fence->Reset();
		}
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GraphicsDeviceD3D12::GetUploadCommandList()
	{
		return m_UploadCommandList;
	}

	void GraphicsDeviceD3D12::SignalAndWait(Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue)
	{
		// Step 1: Increment fence value BEFORE signaling
		++m_FenceValue;

		// Step 2: Signal the fence from the command queue
		HRESULT hr = queue->Signal(m_Fence.Get(), m_FenceValue);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to signal fence. HRESULT: " + std::to_string(hr));
		}

		// Step 3: Set an event to be triggered when the GPU reaches the fence value
		hr = m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to set fence event. HRESULT: " + std::to_string(hr));
		}

		// Step 4: Wait for the event to be signaled (i.e., GPU has completed work)
		DWORD waitResult = WaitForSingleObject(m_FenceEvent, INFINITE);
		if (waitResult != WAIT_OBJECT_0)
		{
			throw std::runtime_error("Failed to wait for fence event. Wait result: " + std::to_string(waitResult));
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

	const D3D12DeviceFeatures &GraphicsDeviceD3D12::GetD3D12DeviceFeatures() const
	{
		return m_D3D12Features;
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
		const AccelerationStructureGeometryBuildDescription &description,
		const std::vector<uint32_t>							&primitiveCount) const
	{
		return AccelerationStructureBuildSizeDescription();
	}

	bool GraphicsDeviceD3D12::IsVersionGreaterThan(D3D_FEATURE_LEVEL level)
	{
		Ref<PhysicalDeviceD3D12> physicalDeviceD3D12 = std::dynamic_pointer_cast<PhysicalDeviceD3D12>(m_PhysicalDevice);
		return physicalDeviceD3D12->IsVersionGreaterThan(level);
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
			SignalAndWait(m_CommandQueue);
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

		// check for depth bounds testing support
		{
			D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2 = {};
			HRESULT							  hr	   = m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, sizeof(options2));
			if (SUCCEEDED(hr))
			{
				if (options2.DepthBoundsTestSupported)
				{
					m_Features.SupportsDepthBoundsTesting = true;
				}
			}
		}

		if (IsVersionGreaterThan(D3D_FEATURE_LEVEL_12_2))
		{
			m_D3D12Features.SupportsPipelineStreams = true;

			D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
			HRESULT							  hr	   = m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7));
			if (SUCCEEDED(hr))
			{
				if (options7.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
				{
					m_Features.SupportsMeshTaskShaders = true;
				}
			}

			D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
			hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12));
			if (SUCCEEDED(hr))
			{
				if (options12.EnhancedBarriersSupported)
				{
					m_D3D12Features.SupportsEnhancedBarriers = true;
				}
			}
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
	}	 // namespace Nexus::Graphics
}	 // namespace Nexus::Graphics
#endif