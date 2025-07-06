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

	class GraphicsDeviceD3D12 : public GraphicsDevice
	{
	  public:
		GraphicsDeviceD3D12(std::shared_ptr<IPhysicalDevice> physicalDevice, Microsoft::WRL::ComPtr<IDXGIFactory7> factory);
		~GraphicsDeviceD3D12();

		virtual void SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence) override;
		virtual void SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) override;

		virtual const std::string				 GetAPIName() override;
		virtual const char						*GetDeviceName() override;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const override;

		virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription &description) override;
		virtual Ref<ComputePipeline>  CreateComputePipeline(const ComputePipelineDescription &description) override;
		virtual Ref<CommandList>	  CreateCommandList(const CommandListSpecification &spec = {}) override;
		virtual Ref<ResourceSet>	  CreateResourceSet(const ResourceSetSpecification &spec) override;

		virtual Ref<Framebuffer>  CreateFramebuffer(const FramebufferSpecification &spec) override;
		virtual Ref<Sampler>	  CreateSampler(const SamplerSpecification &spec) override;
		virtual Ref<TimingQuery>  CreateTimingQuery() override;
		virtual Ref<DeviceBuffer> CreateDeviceBuffer(const DeviceBufferDescription &desc) override;

		virtual ShaderLanguage GetSupportedShaderFormat() override
		{
			return ShaderLanguage::HLSL;
		}

		virtual bool IsBufferUsageSupported(BufferUsage usage) override;

		virtual void		WaitForIdle() override;
		virtual GraphicsAPI GetGraphicsAPI() override;

		virtual float GetUVCorrection()
		{
			return -1.0f;
		}
		virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;
		virtual Ref<Texture>			   CreateTexture(const TextureSpecification &spec) override;
		virtual Ref<Swapchain>			   CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) override;
		virtual Ref<Fence>				   CreateFence(const FenceDescription &desc) override;
		virtual FenceWaitResult			   WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) override;
		virtual void					   ResetFences(Ref<Fence> *fences, uint32_t count) override;
		virtual bool					   IsUVOriginTopLeft() override
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

		virtual bool Validate() override;
		virtual void SetName(const std::string &name) override;

		virtual PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const override;
		virtual const DeviceFeatures &GetPhysicalDeviceFeatures() const override;
		virtual const DeviceLimits	 &GetPhysicalDeviceLimits() const override;
		virtual bool				  IsIndexBufferFormatSupported(IndexBufferFormat format) const override;

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;
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
		Microsoft::WRL::ComPtr<D3D12MA::Allocator> m_Allocator = nullptr;

		DeviceFeatures m_Features = {};
		DeviceLimits   m_Limits	  = {};
	};
}	 // namespace Nexus::Graphics
#endif