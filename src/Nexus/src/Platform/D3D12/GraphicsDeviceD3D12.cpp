#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "AccelerationStructureD3D12.hpp"
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
	#include "TexelBufferD3D12.hpp"
	#include "TextureD3D12.hpp"
	#include "TextureViewD3D12.hpp"
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
			// Create a command queue to submit work to the GPU
			D3D12_COMMAND_QUEUE_DESC commandQueueDesc {};
			commandQueueDesc.Type	  = D3D12_COMMAND_LIST_TYPE_DIRECT;
			commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
			commandQueueDesc.NodeMask = 0;
			commandQueueDesc.Flags	  = D3D12_COMMAND_QUEUE_FLAG_NONE;

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
	}

	const std::string GraphicsDeviceD3D12::GetAPIName()
	{
		return {"D3D12"};
	}

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceD3D12::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	Ref<IShaderModule> GraphicsDeviceD3D12::CreateShaderModule(const ShaderModuleSpecification &moduleSpec)
	{
		return CreateRef<ShaderModuleD3D12>(moduleSpec);
	}

	Ref<IGraphicsPipeline> GraphicsDeviceD3D12::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineD3D12>(this, description);
	}

	Ref<IComputePipeline> GraphicsDeviceD3D12::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		return CreateRef<ComputePipelineD3D12>(this, description);
	}

	Ref<IMeshletPipeline> GraphicsDeviceD3D12::CreateMeshletPipeline(const MeshletPipelineDescription &description)
	{
		return CreateRef<MeshletPipelineD3D12>(this, description);
	}

	Ref<IRayTracingPipeline> GraphicsDeviceD3D12::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
	{
		return Ref<IRayTracingPipeline>();
	}

	Ref<IResourceSet> GraphicsDeviceD3D12::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		return CreateRef<ResourceSetD3D12>(pipeline, this);
	}

	Ref<IFramebuffer> GraphicsDeviceD3D12::CreateFramebuffer(const FramebufferTextureSetDescription &desc)
	{
		return CreateRef<FramebufferD3D12>(desc, this);
	}

	Ref<ISampler> GraphicsDeviceD3D12::CreateSampler(const SamplerDescription &spec)
	{
		return CreateRef<SamplerD3D12>(spec);
	}

	Ref<ITimingQuery> GraphicsDeviceD3D12::CreateTimingQuery()
	{
		return CreateRef<TimingQueryD3D12>(this);
	}

	Ref<IDeviceBuffer> GraphicsDeviceD3D12::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return CreateRef<DeviceBufferD3D12>(desc, this);
	}

	Ref<IAccelerationStructure> GraphicsDeviceD3D12::CreateAccelerationStructure(const AccelerationStructureDescription &desc)
	{
		return CreateRef<AccelerationStructureD3D12>(desc, this);
	}

	Ref<ITexelBuffer> GraphicsDeviceD3D12::CreateTexelBuffer(const TexelBufferDescription &desc)
	{
		return CreateRef<TexelBufferD3D12>(desc);
	}

	Microsoft::WRL::ComPtr<D3D12MA::Allocator> GraphicsDeviceD3D12::GetAllocator()
	{
		return m_Allocator.Get();
	}

	Microsoft::WRL::ComPtr<IDXGIFactory7> GraphicsDeviceD3D12::GetDXGIFactory() const
	{
		return m_DxgiFactory.Get();
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
		for (size_t i = 0; i < m_CreatedCommandQueues.size(); i++)
		{
			WeakRef<CommandQueueD3D12> commandQueue = m_CreatedCommandQueues.at(i);

			// check if the command queue pointer has expired, if it has remove it and continue iterating
			if (commandQueue.expired())
			{
				m_CreatedCommandQueues.erase(m_CreatedCommandQueues.begin() + i);
				i--;
				continue;
			}

			if (Ref<CommandQueueD3D12> lockedQueue = commandQueue.lock())
			{
				lockedQueue->WaitForIdle();
			}
		}
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

	Ref<ITexture> GraphicsDeviceD3D12::CreateTexture(const TextureDescription &spec)
	{
		return CreateRef<TextureD3D12>(spec, this);
	}

	Ref<ITextureView> GraphicsDeviceD3D12::CreateTextureView(const TextureViewDescription &desc)
	{
		return CreateRef<TextureViewD3D12>(desc);
	}

	Ref<IFence> GraphicsDeviceD3D12::CreateFence(const FenceDescription &desc)
	{
		return CreateRef<FenceD3D12>(desc, this);
	}

	FenceWaitResult GraphicsDeviceD3D12::WaitForFences(Ref<IFence> *fences, uint32_t count, bool waitAll, TimeSpan timeout)
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
		info.Capabilities	  = QueueCapabilities(QueueCapabilities::Graphics | QueueCapabilities::Compute | QueueCapabilities::Transfer |
											  QueueCapabilities::SparseBinding | QueueCapabilities::VideoEncode | QueueCapabilities::VideoDecode);

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

	void GraphicsDeviceD3D12::ResetFences(Ref<IFence> *fences, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceD3D12> fence = std::dynamic_pointer_cast<FenceD3D12>(fences[i]);
			fence->Reset();
		}
	}

	bool GraphicsDeviceD3D12::Validate()
	{
		return m_Device && m_DxgiFactory;
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
		const AccelerationStructureGeometryBuildDescription &description) const
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS  inputs	   = {};
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>			  geometry	   = {};

		D3D12::GetD3D12AccelerationStructureInputs(description, inputs, geometry);

		m_Device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

		return AccelerationStructureBuildSizeDescription {.AccelerationStructureSize = prebuildInfo.ResultDataMaxSizeInBytes,
														  .UpdateScratchSize		 = prebuildInfo.UpdateScratchDataSizeInBytes,
														  .BuildScratchSize			 = prebuildInfo.ScratchDataSizeInBytes};
	}

	RayTracingDeviceDescription GraphicsDeviceD3D12::GetRayTracingDeviceDescription() const
	{
		return RayTracingDeviceDescription();
	}

	AccelerationStructureProperties GraphicsDeviceD3D12::GetAccelerationStructureProperties() const
	{
		AccelerationStructureProperties properties				  = {};
		properties.MinAccelerationStructureScratchOffsetAlignment = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT;
		return properties;
	}

	bool GraphicsDeviceD3D12::IsVersionGreaterThan(D3D_FEATURE_LEVEL level)
	{
		Ref<PhysicalDeviceD3D12> physicalDeviceD3D12 = std::dynamic_pointer_cast<PhysicalDeviceD3D12>(m_PhysicalDevice);
		return physicalDeviceD3D12->IsVersionGreaterThan(level);
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
					// m_D3D12Features.SupportsEnhancedBarriers = true;
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