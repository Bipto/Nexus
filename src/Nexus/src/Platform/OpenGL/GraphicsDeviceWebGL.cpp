#include "GraphicsDeviceWebGL.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceWebGL::GraphicsDeviceWebGL(const GraphicsDeviceSpecification &createInfo,
											 Window							   *window,
											 const SwapchainSpecification	   &swapchainSpec)
		: GraphicsDevice(createInfo, window, swapchainSpec)
	{
	}
	GraphicsDeviceWebGL::~GraphicsDeviceWebGL()
	{
	}

	void GraphicsDeviceWebGL::SetFramebuffer(Ref<Framebuffer> framebuffer)
	{
	}

	void GraphicsDeviceWebGL::SetSwapchain(Swapchain *swapchain)
	{
	}

	void GraphicsDeviceWebGL::SubmitCommandList(Ref<CommandList> commandList)
	{
	}

	const std::string GraphicsDeviceWebGL::GetAPIName()
	{
		return std::string();
	}

	const char *GraphicsDeviceWebGL::GetDeviceName()
	{
		return nullptr;
	}

	Ref<Texture2D> GraphicsDeviceWebGL::CreateTexture2D(const Texture2DSpecification &spec)
	{
		return Ref<Texture2D>();
	}

	Ref<Cubemap> GraphicsDeviceWebGL::CreateCubemap(const CubemapSpecification &spec)
	{
		return Ref<Cubemap>();
	}

	Ref<Pipeline> GraphicsDeviceWebGL::CreatePipeline(const PipelineDescription &description)
	{
		return Ref<Pipeline>();
	}

	Ref<CommandList> GraphicsDeviceWebGL::CreateCommandList(const CommandListSpecification &spec)
	{
		return Ref<CommandList>();
	}

	Ref<VertexBuffer> GraphicsDeviceWebGL::CreateVertexBuffer(const BufferDescription &description, const void *data)
	{
		return Ref<VertexBuffer>();
	}

	Ref<IndexBuffer> GraphicsDeviceWebGL::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
	{
		return Ref<IndexBuffer>();
	}

	Ref<UniformBuffer> GraphicsDeviceWebGL::CreateUniformBuffer(const BufferDescription &description, const void *data)
	{
		return Ref<UniformBuffer>();
	}

	Ref<ResourceSet> GraphicsDeviceWebGL::CreateResourceSet(const ResourceSetSpecification &spec)
	{
		return Ref<ResourceSet>();
	}

	Ref<Framebuffer> GraphicsDeviceWebGL::CreateFramebuffer(const FramebufferSpecification &spec)
	{
		return Ref<Framebuffer>();
	}

	Ref<Sampler> GraphicsDeviceWebGL::CreateSampler(const SamplerSpecification &spec)
	{
		return Ref<Sampler>();
	}

	Ref<TimingQuery> GraphicsDeviceWebGL::CreateTimingQuery()
	{
		return Ref<TimingQuery>();
	}

	const GraphicsCapabilities GraphicsDeviceWebGL::GetGraphicsCapabilities() const
	{
		return GraphicsCapabilities();
	}

	ShaderLanguage GraphicsDeviceWebGL::GetSupportedShaderFormat()
	{
		return ShaderLanguage();
	}

	Ref<ShaderModule> GraphicsDeviceWebGL::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
	{
		return Ref<ShaderModule>();
	}

	std::vector<std::string> GraphicsDeviceWebGL::GetSupportedExtensions()
	{
		return std::vector<std::string>();
	}

}	 // namespace Nexus::Graphics
