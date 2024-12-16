#if defined(NX_PLATFORM_OPENGL)

	#include "GraphicsDeviceOpenGL.hpp"

	#include "BufferOpenGL.hpp"
	#include "CommandListOpenGL.hpp"
	#include "PipelineOpenGL.hpp"
	#include "ResourceSetOpenGL.hpp"
	#include "SamplerOpenGL.hpp"
	#include "ShaderModuleOpenGL.hpp"
	#include "TextureOpenGL.hpp"
	#include "TimingQueryOpenGL.hpp"
	#include "SwapchainOpenGL.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(const GraphicsDeviceSpecification &createInfo,
											   IWindow							 *window,
											   const SwapchainSpecification		 &swapchainSpec)
		: GraphicsDevice(createInfo, window, swapchainSpec)
	{
		m_PBuffer	 = GL::CreateOffscreenContext();
		bool success = m_PBuffer->MakeCurrent();
		m_Extensions = GetSupportedExtensions();

		m_APIName	   = std::string("OpenGL - ") + std::string((const char *)glGetString(GL_VERSION));
		m_RendererName = (const char *)glGetString(GL_RENDERER);

		window->CreateSwapchain(this, swapchainSpec);
		// glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
	{
	}

	void GraphicsDeviceOpenGL::SetFramebuffer(Ref<Framebuffer> framebuffer)
	{
		auto fb = std::dynamic_pointer_cast<FramebufferOpenGL>(framebuffer);
		if (framebuffer)
		{
			fb->BindAsDrawBuffer();
			m_BoundFramebuffer = fb;
		}
	}

	void GraphicsDeviceOpenGL::SetSwapchain(Swapchain *swapchain)
	{
		auto glSwapchain = (SwapchainOpenGL *)swapchain;
		if (glSwapchain)
		{
			glSwapchain->BindAsDrawTarget();
		}
	}

	void GraphicsDeviceOpenGL::SubmitCommandList(Ref<CommandList> commandList)
	{
		auto commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);

		const std::vector<Nexus::Graphics::RenderCommandData> &commands = commandListGL->GetCommandData();
		m_CommandExecutor.ExecuteCommands(commands, this);
		m_CommandExecutor.Reset();
	}

	const std::string GraphicsDeviceOpenGL::GetAPIName()
	{
		return m_APIName;
	}

	const char *GraphicsDeviceOpenGL::GetDeviceName()
	{
		return m_RendererName.c_str();
	}

	GL::IOffscreenContext *GraphicsDeviceOpenGL::GetOffscreenContext()
	{
		return m_PBuffer.get();
	}

	Ref<ShaderModule> GraphicsDeviceOpenGL::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
	{
		return CreateRef<ShaderModuleOpenGL>(moduleSpec, resources);
	}

	std::vector<std::string> GraphicsDeviceOpenGL::GetSupportedExtensions()
	{
		std::vector<std::string> extensions;

		GLint n = 0;
		glCall(glGetIntegerv(GL_NUM_EXTENSIONS, &n));

		for (GLint i = 0; i < n; i++)
		{
			const char *extension = (const char *)glGetStringi(GL_EXTENSIONS, i);
			extensions.push_back(extension);
		}

		return extensions;
	}

	Ref<Texture2D> GraphicsDeviceOpenGL::CreateTexture2D(const Texture2DSpecification &spec)
	{
		return CreateRef<Texture2DOpenGL>(spec, this);
	}

	Ref<Cubemap> GraphicsDeviceOpenGL::CreateCubemap(const CubemapSpecification &spec)
	{
		return CreateRef<CubemapOpenGL>(spec, this);
	}

	Ref<Pipeline> GraphicsDeviceOpenGL::CreatePipeline(const PipelineDescription &description)
	{
		return CreateRef<PipelineOpenGL>(description);
	}

	Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList(const CommandListSpecification &spec)
	{
		return CreateRef<CommandListOpenGL>(spec);
	}

	Ref<VertexBuffer> GraphicsDeviceOpenGL::CreateVertexBuffer(const BufferDescription &description, const void *data)
	{
		return CreateRef<VertexBufferOpenGL>(description, data);
	}

	Ref<IndexBuffer> GraphicsDeviceOpenGL::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
	{
		return CreateRef<IndexBufferOpenGL>(description, data, format);
	}

	Ref<UniformBuffer> GraphicsDeviceOpenGL::CreateUniformBuffer(const BufferDescription &description, const void *data)
	{
		return CreateRef<UniformBufferOpenGL>(description, data);
	}

	Ref<ResourceSet> GraphicsDeviceOpenGL::CreateResourceSet(const ResourceSetSpecification &spec)
	{
		return CreateRef<ResourceSetOpenGL>(spec);
	}

	Ref<Framebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(const FramebufferSpecification &spec)
	{
		return CreateRef<FramebufferOpenGL>(spec, this);
	}

	Ref<Sampler> GraphicsDeviceOpenGL::CreateSampler(const SamplerSpecification &spec)
	{
		return CreateRef<SamplerOpenGL>(spec);
	}

	Ref<TimingQuery> GraphicsDeviceOpenGL::CreateTimingQuery()
	{
		return CreateRef<TimingQueryOpenGL>();
	}

	const GraphicsCapabilities GraphicsDeviceOpenGL::GetGraphicsCapabilities() const
	{
		GraphicsCapabilities capabilities;

	#if defined(NX_PLATFORM_GL_DESKTOP)
		capabilities.SupportsLODBias			  = true;
		capabilities.SupportsMultisampledTextures = true;
		capabilities.SupportsMultipleSwapchains	  = true;
	#endif

		return capabilities;
	}

	Swapchain *GraphicsDeviceOpenGL::CreateSwapchain(IWindow *window, const SwapchainSpecification &spec)
	{
		return new SwapchainOpenGL(window, spec, this);
	}

	ShaderLanguage GraphicsDeviceOpenGL::GetSupportedShaderFormat()
	{
	#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(ANDROID)
		return ShaderLanguage::GLSLES;
	#else
		return ShaderLanguage::GLSL;
	#endif
	}
}	 // namespace Nexus::Graphics

#endif