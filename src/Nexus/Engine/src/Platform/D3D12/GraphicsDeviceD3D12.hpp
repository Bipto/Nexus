#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "CommandExecutorD3D12.hpp"
	#include "D3D12Include.hpp"
	#include "DeviceBufferD3D12.hpp"
	#include "RHI/GraphicsDevice.hpp"
	#include "RHI/IPhysicalDevice.hpp"

	#include "CommandQueueD3D12.hpp"

namespace Nexus::Graphics
{
	class TextureD3D12;
	class CommandQueueD3D12;

	struct D3D12DeviceFeatures
	{
		bool SupportsPipelineStreams  = false;
		bool SupportsEnhancedBarriers = false;
	};

	class GraphicsDeviceD3D12 final : public IGraphicsDevice
	{
	  public:
		GraphicsDeviceD3D12(std::shared_ptr<IPhysicalDevice> physicalDevice, Microsoft::WRL::ComPtr<IDXGIFactory7> factory);
		~GraphicsDeviceD3D12();

		std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

		Ref<IGraphicsPipeline>	 CreateGraphicsPipeline(const GraphicsPipelineDescription &description) final;
		Ref<IComputePipeline>	 CreateComputePipeline(const ComputePipelineDescription &description) final;
		Ref<IMeshletPipeline>	 CreateMeshletPipeline(const MeshletPipelineDescription &description) final;
		Ref<IRayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDescription &description) final;
		Ref<IResourceSet>		 CreateResourceSet(Ref<Pipeline> pipeline) final;

		Ref<IFramebuffer>			CreateFramebuffer(const FramebufferTextureSetDescription &desc) final;
		SamplerHandle				CreateSampler(const SamplerDescription &spec) final;
		Ref<ITimingQuery>			CreateTimingQuery() final;
		Ref<IDeviceBuffer>			CreateDeviceBuffer(const DeviceBufferDescription &desc) final;
		Ref<IAccelerationStructure> CreateAccelerationStructure(const AccelerationStructureDescription &desc) final;
		Ref<ITexelBuffer>			CreateTexelBuffer(const TexelBufferDescription &desc) final;

		ShaderLanguage GetSupportedShaderFormat() final
		{
			return ShaderLanguage::HLSL;
		}

		bool IsBufferUsageSupported(BufferUsage usage) final;

		void			WaitForIdle() final;
		GraphicsAPIInfo GetGraphicsAPI() final;

		float GetUVCorrection() final
		{
			return -1.0f;
		}
		const GraphicsCapabilities	 GetGraphicsCapabilities() const final;
		TextureHandle				 CreateTexture(const TextureDescription &spec) final;
		TextureViewHandle			 CreateTextureView(const TextureViewDescription &desc) final;
		Ref<IFence>					 CreateFence(const FenceDescription &desc) final;
		FenceWaitResult				 WaitForFences(Ref<IFence> *fences, uint32_t count, bool waitAll, uint64_t timeoutNS) final;
		std::vector<QueueFamilyInfo> GetQueueFamilies() final;
		Ref<ICommandQueue>			 CreateCommandQueue(const CommandQueueDescription &description) final;
		void						 ResetFences(Ref<IFence> *fences, uint32_t count) final;
		bool						 IsUVOriginTopLeft() final
		{
			return true;
		};

		Microsoft::WRL::ComPtr<D3D12MA::Allocator> GetAllocator();

		Microsoft::WRL::ComPtr<IDXGIFactory7> GetDXGIFactory() const;
		Microsoft::WRL::ComPtr<ID3D12Device9> GetD3D12Device() const;

		bool Validate() final;

		PixelFormatProperties	   GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const final;
		const DeviceFeatures	  &GetPhysicalDeviceFeatures() const final;
		const DeviceLimits		  &GetPhysicalDeviceLimits() const final;
		const D3D12DeviceFeatures &GetD3D12DeviceFeatures() const;

		bool									  IsIndexBufferFormatSupported(IndexFormat format) const final;
		AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(
			const AccelerationStructureGeometryBuildDescription &description) const final;
		RayTracingDeviceDescription		GetRayTracingDeviceDescription() const final;
		AccelerationStructureProperties GetAccelerationStructureProperties() const final;

		Ref<ISurface> CreateSurfaceFromWin32(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance) const final;
		Ref<ISurface> CreateSurfaceFromX11(uintptr_t display, uint32_t screen, uint32_t window) const final;
		Ref<ISurface> CreateSurfaceFromWayland(uintptr_t display, uintptr_t surface) const final;
		Ref<ISurface> CreateSurfaceFromAndroid(uintptr_t nativeWindow) const final;
		Ref<ISurface> CreateSurfaceFromHTML(const std::string &canvasId) const final;

		bool IsVersionGreaterThan(D3D_FEATURE_LEVEL level);

		virtual Ref<IShaderModule> CreateShaderModule(const ShaderModuleDescription &moduleSpec) override;

	  private:
		void			   GetLimitsAndFeatures();
		inline static void ReportLiveObjects();

	  private:
	#if defined(_DEBUG)
		Microsoft::WRL::ComPtr<ID3D12Debug5> m_D3D12Debug = nullptr;
		Microsoft::WRL::ComPtr<IDXGIDebug1>	 m_DXGIDebug  = nullptr;
	#endif

		Microsoft::WRL::ComPtr<ID3D12Device9> m_Device = nullptr;

		Microsoft::WRL::ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;

		std::shared_ptr<IPhysicalDevice> m_PhysicalDevice = nullptr;

		std::unique_ptr<CommandExecutorD3D12>	   m_CommandExecutor = nullptr;
		Microsoft::WRL::ComPtr<D3D12MA::Allocator> m_Allocator		 = nullptr;

		DeviceFeatures		m_Features		= {};
		DeviceLimits		m_Limits		= {};
		D3D12DeviceFeatures m_D3D12Features = {};

		GraphicsResourceManager m_Resources = {};

		std::vector<WeakRef<CommandQueueD3D12>> m_CreatedCommandQueues = {};

		friend class SwapchainD3D12;
	};
}	 // namespace Nexus::Graphics
#endif