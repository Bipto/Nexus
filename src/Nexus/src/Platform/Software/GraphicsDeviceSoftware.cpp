#include "GraphicsDeviceSoftware.hpp"

#include "Platform/Software/CommandExecutorSoftware.hpp"
#include "Platform/Software/CommandListSoftware.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceSoftware::GraphicsDeviceSoftware(const GraphicsDeviceSpecification &createInfo,
												   Window							 *window,
												   const SwapchainSpecification		 &swapchainSpec)
		: GraphicsDevice(createInfo, window, swapchainSpec)
	{
		window->CreateSwapchain(this, swapchainSpec);
	}

	GraphicsDeviceSoftware::~GraphicsDeviceSoftware()
	{
	}

	const std::string GraphicsDeviceSoftware::GetAPIName()
	{
		return std::string();
	}

	const char *GraphicsDeviceSoftware::GetDeviceName()
	{
		return nullptr;
	}

	void GraphicsDeviceSoftware::SubmitCommandList(Ref<CommandList> commandList)
	{
		auto commandListSoftware = std::dynamic_pointer_cast<CommandListSoftware>(commandList);

		const std::vector<Nexus::Graphics::RenderCommandData> &commands = commandListSoftware->GetCommandData();
		m_CommandExecutor.ExecuteCommands(commands, this);
		m_CommandExecutor.Reset();
	}

	Ref<Pipeline> GraphicsDeviceSoftware::CreatePipeline(const PipelineDescription &description)
	{
		return Ref<Pipeline>();
	}

	Ref<VertexBuffer> GraphicsDeviceSoftware::CreateVertexBuffer(const BufferDescription &description, const void *data)
	{
		return Ref<VertexBuffer>();
	}

	Ref<IndexBuffer> GraphicsDeviceSoftware::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
	{
		return Ref<IndexBuffer>();
	}

	Ref<UniformBuffer> GraphicsDeviceSoftware::CreateUniformBuffer(const BufferDescription &description, const void *data)
	{
		return Ref<UniformBuffer>();
	}

	Ref<CommandList> GraphicsDeviceSoftware::CreateCommandList(const CommandListSpecification &spec)
	{
		return CreateRef<CommandListSoftware>(spec);
	}

	Ref<Texture2D> GraphicsDeviceSoftware::CreateTexture2D(const Texture2DSpecification &spec)
	{
		return Ref<Texture2D>();
	}

	Ref<Cubemap> GraphicsDeviceSoftware::CreateCubemap(const CubemapSpecification &spec)
	{
		return Ref<Cubemap>();
	}

	Ref<Framebuffer> GraphicsDeviceSoftware::CreateFramebuffer(const FramebufferSpecification &spec)
	{
		return Ref<Framebuffer>();
	}

	Ref<ResourceSet> GraphicsDeviceSoftware::CreateResourceSet(const ResourceSetSpecification &spec)
	{
		return Ref<ResourceSet>();
	}

	Ref<ResourceSet> GraphicsDeviceSoftware::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		return Ref<ResourceSet>();
	}

	Ref<Sampler> GraphicsDeviceSoftware::CreateSampler(const SamplerSpecification &spec)
	{
		return Ref<Sampler>();
	}

	Ref<TimingQuery> GraphicsDeviceSoftware::CreateTimingQuery()
	{
		return Ref<TimingQuery>();
	}

	ShaderLanguage GraphicsDeviceSoftware::GetSupportedShaderFormat()
	{
		return ShaderLanguage();
	}

	float GraphicsDeviceSoftware::GetUVCorrection()
	{
		return 0.0f;
	}

	bool GraphicsDeviceSoftware::IsUVOriginTopLeft()
	{
		return false;
	}

	const GraphicsCapabilities GraphicsDeviceSoftware::GetGraphicsCapabilities() const
	{
		return GraphicsCapabilities();
	}

	Ref<ShaderModule> GraphicsDeviceSoftware::CreateShaderModule(const ShaderModuleSpecification &moduleSpec,
																 const ResourceSetSpecification	 &resources)
	{
		return Ref<ShaderModule>();
	}
}	 // namespace Nexus::Graphics
