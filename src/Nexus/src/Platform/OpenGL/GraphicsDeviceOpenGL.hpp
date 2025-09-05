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

		const std::string				 GetAPIName() override;
		std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

		Ref<GraphicsPipeline>	CreateGraphicsPipeline(const GraphicsPipelineDescription &description) final;
		Ref<ComputePipeline>	CreateComputePipeline(const ComputePipelineDescription &description) final;
		Ref<MeshletPipeline>	CreateMeshletPipeline(const MeshletPipelineDescription &description) final;
		Ref<RayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDescription &description) final;

		Ref<CommandList>			CreateCommandList(const CommandListDescription &spec = {}) final;
		Ref<ResourceSet>			CreateResourceSet(Ref<Pipeline> pipeline) final;
		Ref<Framebuffer>			CreateFramebuffer(const FramebufferSpecification &spec) final;
		Ref<Sampler>				CreateSampler(const SamplerDescription &spec) final;
		Ref<TimingQuery>			CreateTimingQuery() final;
		Ref<DeviceBuffer>			CreateDeviceBuffer(const DeviceBufferDescription &desc) final;
		Ref<IAccelerationStructure> CreateAccelerationStructure(const AccelerationStructureDescription &desc) final;

		const GraphicsCapabilities	 GetGraphicsCapabilities() const final;
		Ref<Texture>				 CreateTexture(const TextureDescription &spec) final;
		Ref<Swapchain>				 CreateSwapchain(IWindow *window, const SwapchainDescription &spec) final;
		Ref<Fence>					 CreateFence(const FenceDescription &desc) final;
		FenceWaitResult				 WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) final;
		std::vector<QueueFamilyInfo> GetQueueFamilies() final;
		Ref<ICommandQueue>			 CreateCommandQueue(const CommandQueueDescription &description) final;
		void						 ResetFences(Ref<Fence> *fences, uint32_t count) final;
		ShaderLanguage				 GetSupportedShaderFormat() final;
		bool						 IsBufferUsageSupported(BufferUsage usage) final;
		void						 WaitForIdle() final;
		float						 GetUVCorrection() final
		{
			return 1.0f;
		}
		bool IsUVOriginTopLeft() final
		{
			return false;
		};

		GraphicsAPI GetGraphicsAPI() final;

		bool Validate() final;

		GL::IOffscreenContext *GetOffscreenContext();

		const DeviceFeatures					 &GetPhysicalDeviceFeatures() const final;
		const DeviceLimits						 &GetPhysicalDeviceLimits() const final;
		bool									  IsIndexBufferFormatSupported(IndexFormat format) const final;
		AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(const AccelerationStructureGeometryBuildDescription &description,
																					const std::vector<uint32_t> &primitiveCount) const final;

		Ref<PhysicalDeviceOpenGL> GetPhysicalDeviceOpenGL();

	  private:
		Ref<ShaderModule>		 CreateShaderModule(const ShaderModuleSpecification &moduleSpec) final;
		std::vector<std::string> GetSupportedExtensions(const GladGLContext &context);

		PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const final;

	  private:
		WeakRef<FramebufferOpenGL> m_BoundFramebuffer = {};

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