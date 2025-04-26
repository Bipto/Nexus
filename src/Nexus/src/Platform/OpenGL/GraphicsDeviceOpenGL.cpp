#if defined(NX_PLATFORM_OPENGL)

	#include "GraphicsDeviceOpenGL.hpp"

	#include "DeviceBufferOpenGL.hpp"
	#include "CommandListOpenGL.hpp"
	#include "PipelineOpenGL.hpp"
	#include "ResourceSetOpenGL.hpp"
	#include "SamplerOpenGL.hpp"
	#include "ShaderModuleOpenGL.hpp"
	#include "TextureOpenGL.hpp"
	#include "TimingQueryOpenGL.hpp"
	#include "SwapchainOpenGL.hpp"
	#include "DeviceBufferOpenGL.hpp"

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	std::cout << "OpenGL Debug Message: " << message << std::endl;
}

namespace Nexus::Graphics
{
	GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(std::shared_ptr<IPhysicalDevice> physicalDevice, bool enableDebug)
	{
		m_PhysicalDevice = std::dynamic_pointer_cast<PhysicalDeviceOpenGL>(physicalDevice);
		m_Extensions = GetSupportedExtensions();

		m_APIName	   = std::string("OpenGL - ") + std::string((const char *)glGetString(GL_VERSION));
		m_RendererName = (const char *)glGetString(GL_RENDERER);

		// glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		if (enableDebug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(debugCallback, nullptr);
			glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		}
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

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceOpenGL::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	GL::IOffscreenContext *GraphicsDeviceOpenGL::GetOffscreenContext()
	{
		return m_PhysicalDevice->GetOffscreenContext();
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

	Ref<GraphicsPipeline> GraphicsDeviceOpenGL::CreatePipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineOpenGL>(description);
	}

	Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList(const CommandListSpecification &spec)
	{
		return CreateRef<CommandListOpenGL>(spec);
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

	DeviceBuffer *GraphicsDeviceOpenGL::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return new DeviceBufferOpenGL(desc);
	}

	void GraphicsDeviceOpenGL::CopyBuffer(const BufferCopyDescription &desc)
	{
		DeviceBufferOpenGL *src = (DeviceBufferOpenGL *)desc.Source;
		DeviceBufferOpenGL *dst = (DeviceBufferOpenGL *)desc.Target;

		glBindBuffer(GL_COPY_READ_BUFFER, src->GetBufferHandle());
		glBindBuffer(GL_COPY_WRITE_BUFFER, dst->GetBufferHandle());

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, desc.ReadOffset, desc.WriteOffset, desc.Size);

		glBindBuffer(GL_COPY_READ_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	const GraphicsCapabilities GraphicsDeviceOpenGL::GetGraphicsCapabilities() const
	{
		GraphicsCapabilities capabilities;

	#if defined(NX_PLATFORM_GL_DESKTOP)
		capabilities.SupportsLODBias			  = true;
		capabilities.SupportsMultisampledTextures = true;
		capabilities.SupportsMultipleSwapchains	  = true;
		capabilities.SupportsSeparateColourAndBlendMasks = true;
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

	void GraphicsDeviceOpenGL::WaitForIdle()
	{
		glFinish();
	}

	GraphicsAPI GraphicsDeviceOpenGL::GetGraphicsAPI()
	{
		return GraphicsAPI::OpenGL;
	}

	bool GraphicsDeviceOpenGL::Validate()
	{
		GL::IOffscreenContext *context = m_PhysicalDevice->GetOffscreenContext();
		return context->Validate();
	}
}	 // namespace Nexus::Graphics

#endif