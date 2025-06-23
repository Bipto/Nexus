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
	#include "FenceOpenGL.hpp"

	#if !defined(__EMSCRIPTEN__)
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		std::cout << "OpenGL Debug Message: " << message << std::endl;
	}
}
	#endif

namespace Nexus::Graphics
{
	GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(std::shared_ptr<IPhysicalDevice> physicalDevice, bool enableDebug)
	{
		m_PhysicalDevice = std::dynamic_pointer_cast<PhysicalDeviceOpenGL>(physicalDevice);
		m_Extensions	 = GetSupportedExtensions();

		m_APIName	   = std::string("OpenGL - ") + std::string((const char *)glGetString(GL_VERSION));
		m_RendererName = (const char *)glGetString(GL_RENDERER);

	#if !defined(__EMSCRIPTEN__)
		if (enableDebug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(debugCallback, nullptr);
		}
	#endif
	}

	GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
	{
	}

	void GraphicsDeviceOpenGL::SetFramebuffer(WeakRef<Framebuffer> framebuffer)
	{
		if (Ref<Framebuffer> fb = framebuffer.lock())
		{
			auto framebufferGL = std::dynamic_pointer_cast<FramebufferOpenGL>(fb);
			if (framebufferGL)
			{
				framebufferGL->BindAsDrawBuffer();
				m_BoundFramebuffer = framebufferGL;
			}
		}
	}

	void GraphicsDeviceOpenGL::SetSwapchain(WeakRef<Swapchain> swapchain)
	{
		if (Ref<Swapchain> sc = swapchain.lock())
		{
			auto glSwapchain = std::dynamic_pointer_cast<SwapchainOpenGL>(sc);
			if (glSwapchain)
			{
				glSwapchain->BindAsDrawTarget();
			}
		}
	}

	void GraphicsDeviceOpenGL::SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence)
	{
		Ref<CommandListOpenGL>								   commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
		const std::vector<Nexus::Graphics::RenderCommandData> &commands		 = commandListGL->GetCommandData();
		m_CommandExecutor.ExecuteCommands(commands, this);
		m_CommandExecutor.Reset();
	}

	void GraphicsDeviceOpenGL::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
	{
		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListOpenGL>								   commandList = std::dynamic_pointer_cast<CommandListOpenGL>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			m_CommandExecutor.ExecuteCommands(commands, this);
			m_CommandExecutor.Reset();
		}
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

	void GraphicsDeviceOpenGL::CopyBufferToTexture(Ref<TextureOpenGL>	   texture,
												   Ref<DeviceBufferOpenGL> buffer,
												   uint32_t				   bufferOffset,
												   SubresourceDescription  subresource)
	{
		NX_ASSERT(texture->GetSpecification().Samples == 1, "Cannot set data in a multisampled texture");

		if (subresource.Depth > 1)
		{
			NX_ASSERT(texture->GetSpecification().Type == TextureType::Texture3D,
					  "Attempting to set data in a multi-layer texture, but texture is not multi layer");
		}

		GLenum textureType = texture->GetTextureType();
		GLenum dataFormat  = texture->GetDataFormat();
		GLenum baseType	   = texture->GetBaseType();

		glCall(glBindTexture(textureType, texture->GetHandle()));
		buffer->Bind(GL_PIXEL_UNPACK_BUFFER);

		GLenum	 glAspect	= GL::GetGLImageAspect(subresource.Aspect);
		uint32_t bufferSize = (subresource.Width - subresource.X) * (subresource.Height - subresource.Y) *
							  (uint32_t)GetPixelFormatSizeInBytes(texture->GetSpecification().Format);

		switch (texture->GetInternalGLTextureFormat())
		{
			case GL::GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(glTexSubImage1D(textureType,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Width,
									   dataFormat,
									   baseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
	#else
				throw std::runtime_error("1D textures are not supported in WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Texture2DMultisample:
				glCall(glTexSubImage2D(textureType,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Y,
									   subresource.Width,
									   subresource.Height,
									   dataFormat,
									   baseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + subresource.ArrayLayer,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Y,
									   subresource.Width,
									   subresource.Height,
									   dataFormat,
									   baseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
				glCall(glTexSubImage3D(textureType,
									   subresource.MipLevel,
									   subresource.X,
									   subresource.Y,
									   subresource.ArrayLayer,
									   subresource.Width,
									   subresource.Height,
									   subresource.Depth,
									   dataFormat,
									   baseType,
									   (const void *)(uint64_t)bufferOffset));
				break;
		}

		GL::ClearBufferBinding(GL_PIXEL_UNPACK_BUFFER);
		glCall(glBindTexture(textureType, 0));
	}

	void GraphicsDeviceOpenGL::CopyTextureToBuffer(Ref<TextureOpenGL>	   texture,
												   Ref<DeviceBufferOpenGL> buffer,
												   uint32_t				   bufferOffset,
												   SubresourceDescription  subresource)
	{
		const auto &textureSpecification = texture->GetSpecification();

		size_t layerSize =
			(subresource.Width - subresource.X) * (subresource.Height - subresource.Y) * GetPixelFormatSizeInBytes(textureSpecification.Format);
		size_t bufferSize = layerSize * subresource.Depth;
		GLenum glAspect	  = GL::GetGLImageAspect(subresource.Aspect);

		GLenum textureType = texture->GetTextureType();
		GLenum dataFormat  = texture->GetDataFormat();
		GLenum baseType	   = texture->GetBaseType();

		buffer->Bind(GL_PIXEL_PACK_BUFFER);

		for (uint32_t layer = subresource.Z; layer < subresource.Depth; layer++)
		{
			GLuint framebufferHandle = 0;
			glCall(glGenFramebuffers(1, &framebufferHandle));
			glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle));
			GL::AttachTexture(framebufferHandle, texture, subresource.MipLevel, layer, subresource.Aspect, 0);

			GL::ValidateFramebuffer(framebufferHandle);

			glCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
			glCall(glReadPixels(subresource.X,
								subresource.Y,
								subresource.Width,
								subresource.Height,
								dataFormat,
								baseType,
								(void *)(uint64_t)bufferOffset));

			glCall(glFlush());
			glCall(glFinish());

			glCall(glDeleteFramebuffers(1, &framebufferHandle));
			bufferOffset += layerSize;
		}

		glCall(glBindTexture(textureType, 0));
		glCall(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
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

	PixelFormatProperties GraphicsDeviceOpenGL::GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const
	{
		PixelFormatProperties properties = {};
		return properties;
	}

	const DeviceFeatures &GraphicsDeviceOpenGL::GetPhysicalDeviceFeatures() const
	{
		return m_Features;
	}

	const DeviceLimits &GraphicsDeviceOpenGL::GetPhysicalDeviceLimits() const
	{
		return m_Limits;
	}

	Ref<GraphicsPipeline> GraphicsDeviceOpenGL::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineOpenGL>(description);
	}

	Ref<ComputePipeline> GraphicsDeviceOpenGL::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		return CreateRef<ComputePipelineOpenGL>(description);
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

	Ref<DeviceBuffer> GraphicsDeviceOpenGL::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return CreateRef<DeviceBufferOpenGL>(desc);
	}

	const GraphicsCapabilities GraphicsDeviceOpenGL::GetGraphicsCapabilities() const
	{
		GraphicsCapabilities capabilities;

	#if defined(NX_PLATFORM_GL_DESKTOP)
		capabilities.SupportsLODBias					 = true;
		capabilities.SupportsMultisampledTextures		 = true;
		capabilities.SupportsMultipleSwapchains			 = true;
		capabilities.SupportsSeparateColourAndBlendMasks = true;
	#endif

		return capabilities;
	}

	Ref<Swapchain> GraphicsDeviceOpenGL::CreateSwapchain(IWindow *window, const SwapchainSpecification &spec)
	{
		return CreateRef<SwapchainOpenGL>(window, spec, this);
	}

	Ref<Fence> GraphicsDeviceOpenGL::CreateFence(const FenceDescription &desc)
	{
		return CreateRef<FenceOpenGL>(desc);
	}

	FenceWaitResult GraphicsDeviceOpenGL::WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout)
	{
		std::vector<FenceWaitResult> success(count);

		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceOpenGL> fence = std::dynamic_pointer_cast<FenceOpenGL>(fences[i]);

			GLenum result = fence->Wait(timeout);
			if (result == GL_ALREADY_SIGNALED || GL_CONDITION_SATISFIED)
			{
				// if a fence has been signalled successfully and we are not waiting for all the fences, we can return that a fence has been signalled
				if (!waitAll)
				{
					return FenceWaitResult::Signalled;
				}

				success[i] = FenceWaitResult::Signalled;
			}
			else if (result == GL_TIMEOUT_EXPIRED)
			{
				success[i] = FenceWaitResult::TimedOut;
			}
			else
			{
				success[i] = FenceWaitResult::Failed;
			}
		}

		// if we are waiting for all fences, we need to check that they have all been completed
		bool allCompleted	  = true;
		bool errorEncountered = false;
		for (size_t i = 0; i < success.size(); i++)
		{
			if (success[i] != FenceWaitResult::Signalled)
			{
				allCompleted = false;
			}

			if (success[i] == FenceWaitResult::Failed)
			{
				errorEncountered = true;
			}
		}

		if (!errorEncountered)
		{
			if (allCompleted)
			{
				return FenceWaitResult::Signalled;
			}
			else
			{
				return FenceWaitResult::TimedOut;
			}
		}

		return FenceWaitResult::Failed;
	}

	void GraphicsDeviceOpenGL::ResetFences(Ref<Fence> *fences, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceOpenGL> fence = std::dynamic_pointer_cast<FenceOpenGL>(fences[i]);
			fence->Reset();
		}
	}

	Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(const TextureSpecification &spec)
	{
		return CreateRef<TextureOpenGL>(spec, this);
	}

	ShaderLanguage GraphicsDeviceOpenGL::GetSupportedShaderFormat()
	{
	#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(ANDROID)
		return ShaderLanguage::GLSLES;
	#else
		return ShaderLanguage::GLSL;
	#endif
	}

	bool GraphicsDeviceOpenGL::IsBufferUsageSupported(BufferUsage usage)
	{
		return false;
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