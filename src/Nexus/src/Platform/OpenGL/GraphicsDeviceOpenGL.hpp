#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

	#include "CommandExecutorOpenGL.hpp"
	#include "FramebufferOpenGL.hpp"
	#include "GL.hpp"
	#include "SwapchainOpenGL.hpp"

	#include "PhysicalDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL : public GraphicsDevice
	{
	  public:
		GraphicsDeviceOpenGL(std::shared_ptr<IPhysicalDevice> physicalDevicel, bool enableDebug);
		GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL &) = delete;
		virtual ~GraphicsDeviceOpenGL();

		virtual void SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence) override;
		virtual void SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) override;

		virtual const std::string				 GetAPIName() override;
		virtual const char						*GetDeviceName() override;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

		virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription &description) override;
		virtual Ref<ComputePipeline>  CreateComputePipeline(const ComputePipelineDescription &description) override;
		virtual Ref<CommandList>	  CreateCommandList(const CommandListDescription &spec = {}) override;
		virtual Ref<ResourceSet>	  CreateResourceSet(const ResourceSetSpecification &spec) override;
		virtual Ref<Framebuffer>	  CreateFramebuffer(const FramebufferSpecification &spec) override;
		virtual Ref<Sampler>		  CreateSampler(const SamplerDescription &spec) override;
		virtual Ref<TimingQuery>	  CreateTimingQuery() override;
		virtual Ref<DeviceBuffer>	  CreateDeviceBuffer(const DeviceBufferDescription &desc) override;

		virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;
		virtual Ref<Texture>			   CreateTexture(const TextureDescription &spec) override;
		virtual Ref<Swapchain>			   CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) override;
		virtual Ref<Fence>				   CreateFence(const FenceDescription &desc) override;
		virtual FenceWaitResult			   WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) override;
		virtual void					   ResetFences(Ref<Fence> *fences, uint32_t count) override;
		virtual ShaderLanguage			   GetSupportedShaderFormat() override;
		virtual bool					   IsBufferUsageSupported(BufferUsage usage) override;
		virtual void					   WaitForIdle() override;
		virtual float					   GetUVCorrection() override
		{
			return 1.0f;
		}
		virtual bool IsUVOriginTopLeft() override
		{
			return false;
		};

		virtual GraphicsAPI GetGraphicsAPI() override;

		virtual bool Validate() override;

		GL::IOffscreenContext *GetOffscreenContext();

		virtual const DeviceFeatures &GetPhysicalDeviceFeatures() const override;
		virtual const DeviceLimits	 &GetPhysicalDeviceLimits() const override;
		virtual bool							  IsIndexBufferFormatSupported(IndexFormat format) const override;
		AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(const AccelerationStructureBuildDescription &description,
																					const std::vector<uint32_t> &primitiveCount) const final;

		Ref<PhysicalDeviceOpenGL> GetPhysicalDeviceOpenGL();

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;
		std::vector<std::string>  GetSupportedExtensions(const GladGLContext &context);

		virtual PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const override;

	  private:
		const char				  *m_GlslVersion;
		WeakRef<FramebufferOpenGL> m_BoundFramebuffer = {};
		VSyncState				   m_VsyncState		  = VSyncState::Enabled;

		std::vector<std::string> m_Extensions {};

		CommandExecutorOpenGL m_CommandExecutor {};

		std::string							  m_APIName		   = {};
		std::string							  m_RendererName   = {};
		std::shared_ptr<PhysicalDeviceOpenGL> m_PhysicalDevice = nullptr;

		DeviceFeatures m_Features = {};
		DeviceLimits   m_Limits	  = {};
	};
}	 // namespace Nexus::Graphics

#endif