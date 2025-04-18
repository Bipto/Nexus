#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "CommandExecutorD3D12.hpp"
	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"

namespace Nexus::Graphics
{
	class Texture2D_D3D12;

	class GraphicsDeviceD3D12 : public GraphicsDevice
	{
	  public:
		GraphicsDeviceD3D12(std::shared_ptr<IPhysicalDevice> physicalDevice, Microsoft::WRL::ComPtr<IDXGIFactory7> factory);
		~GraphicsDeviceD3D12();

		virtual void SubmitCommandList(Ref<CommandList> commandList) override;

		virtual const std::string GetAPIName() override;
		virtual const char		 *GetDeviceName() override;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const override;

		virtual Ref<Texture2D>	 CreateTexture2D(const Texture2DSpecification &spec) override;
		virtual Ref<Cubemap>	 CreateCubemap(const CubemapSpecification &spec) override;
		virtual Ref<Pipeline>	 CreatePipeline(const PipelineDescription &description) override;
		virtual Ref<CommandList> CreateCommandList(const CommandListSpecification &spec = {}) override;

		virtual Ref<VertexBuffer>  CreateVertexBuffer(const BufferDescription &description, const void *data) override;
		virtual Ref<IndexBuffer>   CreateIndexBuffer(const BufferDescription &description,
													 const void				 *data,
													 IndexBufferFormat		  format = IndexBufferFormat::UInt32) override;
		virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;
		virtual Ref<ResourceSet>   CreateResourceSet(const ResourceSetSpecification &spec) override;

		virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) override;
		virtual Ref<Sampler>	 CreateSampler(const SamplerSpecification &spec) override;
		virtual Ref<TimingQuery> CreateTimingQuery() override;
		virtual DeviceBuffer	*CreateDeviceBuffer(const DeviceBufferDescription &desc) override;

		virtual ShaderLanguage GetSupportedShaderFormat() override
		{
			return ShaderLanguage::HLSL;
		}

		virtual void WaitForIdle() override;
		virtual GraphicsAPI GetGraphicsAPI() override;

		virtual float GetUVCorrection()
		{
			return -1.0f;
		}
		virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;
		virtual Swapchain				  *CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) override;
		virtual bool					   IsUVOriginTopLeft() override
		{
			return true;
		};

		IDXGIFactory7			   *GetDXGIFactory() const;
		ID3D12CommandQueue		   *GetCommandQueue() const;
		ID3D12Device9			   *GetDevice() const;
		ID3D12GraphicsCommandList6 *GetUploadCommandList();

		void SignalAndWait();
		void ImmediateSubmit(std::function<void(ID3D12GraphicsCommandList6 *cmd)> &&function);
		void ResourceBarrier(ID3D12GraphicsCommandList6 *cmd,
							 ID3D12Resource				*resource,
							 uint32_t					 level,
							 D3D12_RESOURCE_STATES		 before,
							 D3D12_RESOURCE_STATES		 after);
		void ResourceBarrier(ID3D12GraphicsCommandList6 *cmd, Ref<Texture2D_D3D12> resource, uint32_t level, D3D12_RESOURCE_STATES after);
		void ResourceBarrierSwapchainColour(ID3D12GraphicsCommandList6 *cmd, SwapchainD3D12 *resource, D3D12_RESOURCE_STATES after);
		void ResourceBarrierSwapchainDepth(ID3D12GraphicsCommandList6 *cmd, SwapchainD3D12 *resource, D3D12_RESOURCE_STATES after);

		virtual bool Validate() override;
		virtual void SetName(const std::string &name) override;

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;
		void					  InitUploadCommandList();
		void					  DispatchUploadCommandList();

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
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_UploadCommandList		= nullptr;

		Microsoft::WRL::ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;

		std::shared_ptr<IPhysicalDevice> m_PhysicalDevice = nullptr;

		CommandExecutorD3D12 m_CommandExecutor {};
	};
}	 // namespace Nexus::Graphics
#endif