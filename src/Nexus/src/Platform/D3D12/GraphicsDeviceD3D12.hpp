#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "CommandExecutorD3D12.hpp"
	#include "DeviceBufferD3D12.hpp"
	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"

namespace Nexus::Graphics
{
	class TextureD3D12;

	struct D3D12DeviceFeatures
	{
		bool SupportsPipelineStreams = false;
	};

	class GraphicsDeviceD3D12 final : public GraphicsDevice
	{
	  public:
		GraphicsDeviceD3D12(std::shared_ptr<IPhysicalDevice> physicalDevice, Microsoft::WRL::ComPtr<IDXGIFactory7> factory);
		~GraphicsDeviceD3D12();

		void SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) final;

		const std::string				 GetAPIName() final;
		std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

		Ref<GraphicsPipeline>	CreateGraphicsPipeline(const GraphicsPipelineDescription &description) final;
		Ref<ComputePipeline>	CreateComputePipeline(const ComputePipelineDescription &description) final;
		Ref<MeshletPipeline>	CreateMeshletPipeline(const MeshletPipelineDescription &description) final;
		Ref<RayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDescription &description) final;
		Ref<CommandList>		CreateCommandList(const CommandListDescription &spec = {}) final;
		Ref<ResourceSet>		CreateResourceSet(Ref<Pipeline> pipeline) final;

		Ref<Framebuffer>			CreateFramebuffer(const FramebufferSpecification &spec) final;
		Ref<Sampler>				CreateSampler(const SamplerDescription &spec) final;
		Ref<TimingQuery>			CreateTimingQuery() final;
		Ref<DeviceBuffer>			CreateDeviceBuffer(const DeviceBufferDescription &desc) final;
		Ref<IAccelerationStructure> CreateAccelerationStructure(const AccelerationStructureDescription &desc) final;

		ShaderLanguage GetSupportedShaderFormat() final
		{
			return ShaderLanguage::HLSL;
		}

		bool IsBufferUsageSupported(BufferUsage usage) final;

		void		WaitForIdle() final;
		GraphicsAPI GetGraphicsAPI() final;

		float GetUVCorrection() final
		{
			return -1.0f;
		}
		const GraphicsCapabilities GetGraphicsCapabilities() const final;
		Ref<Texture>			   CreateTexture(const TextureDescription &spec) final;
		Ref<Swapchain>			   CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) final;
		Ref<Fence>				   CreateFence(const FenceDescription &desc) final;
		FenceWaitResult			   WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) final;
		void					   ResetFences(Ref<Fence> *fences, uint32_t count) final;
		bool					   IsUVOriginTopLeft() final
		{
			return true;
		};

		Microsoft::WRL::ComPtr<D3D12MA::Allocator> GetAllocator();

		Microsoft::WRL::ComPtr<IDXGIFactory7>			   GetDXGIFactory() const;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		   GetCommandQueue() const;
		Microsoft::WRL::ComPtr<ID3D12Device9>			   GetDevice() const;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GetUploadCommandList();

		void SignalAndWait();
		void ImmediateSubmit(std::function<void(ID3D12GraphicsCommandList7 *cmd)> &&function);
		void ResourceBarrier(ID3D12GraphicsCommandList7 *cmd,
							 ID3D12Resource				*resource,
							 uint32_t					 layer,
							 uint32_t					 level,
							 uint32_t					 mipCount,
							 D3D12_RESOURCE_STATES		 before,
							 D3D12_RESOURCE_STATES		 after);

		void ResourceBarrier(ID3D12GraphicsCommandList7 *cmd,
							 Ref<TextureD3D12>			 resource,
							 uint32_t					 layer,
							 uint32_t					 level,
							 D3D12_RESOURCE_STATES		 after);

		void ResourceBarrierBuffer(ID3D12GraphicsCommandList7 *cmd,
								   Ref<DeviceBufferD3D12>	   buffer,
								   D3D12_RESOURCE_STATES	   before,
								   D3D12_RESOURCE_STATES	   after);

		void ResourceBarrierSwapchainColour(ID3D12GraphicsCommandList7 *cmd, Ref<SwapchainD3D12> resource, D3D12_RESOURCE_STATES after);
		void ResourceBarrierSwapchainDepth(ID3D12GraphicsCommandList7 *cmd, Ref<SwapchainD3D12> resource, D3D12_RESOURCE_STATES after);

		bool Validate() final;

		PixelFormatProperties					  GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const final;
		const DeviceFeatures					 &GetPhysicalDeviceFeatures() const final;
		const DeviceLimits						 &GetPhysicalDeviceLimits() const final;
		const D3D12DeviceFeatures				 &GetD3D12DeviceFeatures() const;

		bool									  IsIndexBufferFormatSupported(IndexFormat format) const final;
		AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(const AccelerationStructureGeometryBuildDescription &description,
																					const std::vector<uint32_t> &primitiveCounts) const final;

		bool IsVersionGreaterThan(D3D_FEATURE_LEVEL level);

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec) override;
		void					  InitUploadCommandList();
		void					  DispatchUploadCommandList();

		void GetLimitsAndFeatures();

		inline static void ReportLiveObjects();

	  private:
	#if defined(_DEBUG)
		Microsoft::WRL::ComPtr<ID3D12Debug5> m_D3D12Debug = nullptr;
		Microsoft::WRL::ComPtr<IDXGIDebug1>	 m_DXGIDebug  = nullptr;
	#endif

		Microsoft::WRL::ComPtr<ID3D12Device9>	   m_Device		  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence	  = nullptr;
		uint64_t							 m_FenceValue = 0;
		HANDLE								 m_FenceEvent = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	   m_UploadCommandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_UploadCommandList		= nullptr;

		Microsoft::WRL::ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;

		std::shared_ptr<IPhysicalDevice> m_PhysicalDevice = nullptr;

		std::unique_ptr<CommandExecutorD3D12>	   m_CommandExecutor = nullptr;
		Microsoft::WRL::ComPtr<D3D12MA::Allocator> m_Allocator		 = nullptr;

		DeviceFeatures m_Features = {};
		DeviceLimits   m_Limits	  = {};
		D3D12DeviceFeatures m_D3D12Features = {};
	};
}	 // namespace Nexus::Graphics
#endif