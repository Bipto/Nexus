#if defined(NX_PLATFORM_OPENGL)

	#include "GraphicsDeviceOpenGL.hpp"

	#include "CommandListOpenGL.hpp"
	#include "DeviceBufferOpenGL.hpp"
	#include "FenceOpenGL.hpp"
	#include "PipelineOpenGL.hpp"
	#include "ResourceSetOpenGL.hpp"
	#include "SamplerOpenGL.hpp"
	#include "ShaderModuleOpenGL.hpp"
	#include "SwapchainOpenGL.hpp"
	#include "TextureOpenGL.hpp"
	#include "TimingQueryOpenGL.hpp"

	#include "glad/gl.h"

namespace Nexus::Graphics
{
	void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{
		if (type == GL_DEBUG_TYPE_ERROR)
		{
			std::cout << "OpenGL Debug Message: " << message << std::endl;
		}
	}

	GraphicsDeviceOpenGL::GraphicsDeviceOpenGL(std::shared_ptr<IPhysicalDevice> physicalDevice, bool enableDebug)
	{
		m_PhysicalDevice = std::dynamic_pointer_cast<PhysicalDeviceOpenGL>(physicalDevice);
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				m_Extensions = GetSupportedExtensions(context);

				m_APIName	   = std::string("OpenGL - ") + std::string((const char *)context.GetString(GL_VERSION));
				m_RendererName = (const char *)context.GetString(GL_RENDERER);

	#if !defined(__EMSCRIPTEN__)
				if (enableDebug)
				{
					context.Enable(GL_DEBUG_OUTPUT);
					context.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
					context.DebugMessageCallback(glDebugCallback, nullptr);
				}
	#endif
			});
	}

	GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
	{
	}

	void GraphicsDeviceOpenGL::SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());

		Ref<CommandListOpenGL>								   commandListGL = std::dynamic_pointer_cast<CommandListOpenGL>(commandList);
		const std::vector<Nexus::Graphics::RenderCommandData> &commands		 = commandListGL->GetCommandData();
		m_CommandExecutor.ExecuteCommands(commands, this);
		m_CommandExecutor.Reset();

		GL::IOffscreenContext *offscreenContext = m_PhysicalDevice->GetOffscreenContext();
		offscreenContext->MakeCurrent();
	}

	void GraphicsDeviceOpenGL::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());

		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListOpenGL>								   commandList = std::dynamic_pointer_cast<CommandListOpenGL>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			m_CommandExecutor.ExecuteCommands(commands, this);
			m_CommandExecutor.Reset();
		}

		GL::IOffscreenContext *offscreenContext = m_PhysicalDevice->GetOffscreenContext();
		offscreenContext->MakeCurrent();
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

	Ref<ShaderModule> GraphicsDeviceOpenGL::CreateShaderModule(const ShaderModuleSpecification &moduleSpec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<ShaderModuleOpenGL>(moduleSpec, this);
	}

	std::vector<std::string> GraphicsDeviceOpenGL::GetSupportedExtensions(const GladGLContext &context)
	{
		std::vector<std::string> extensions;

		GLint n = 0;
		glCall(context.GetIntegerv(GL_NUM_EXTENSIONS, &n));

		for (GLint i = 0; i < n; i++)
		{
			const char *extension = (const char *)context.GetStringi(GL_EXTENSIONS, i);
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

	bool GraphicsDeviceOpenGL::IsIndexBufferFormatSupported(IndexFormat format) const
	{
		switch (format)
		{
			case IndexFormat::UInt8:
			case IndexFormat::UInt16:
			case IndexFormat::UInt32: return true;

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	AccelerationStructureBuildSizeDescription GraphicsDeviceOpenGL::GetAccelerationStructureBuildSize(
		const AccelerationStructureGeometryBuildDescription &description,
		const std::vector<uint32_t>							&primitiveCount) const
	{
		NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
		return AccelerationStructureBuildSizeDescription();
	}

	Ref<PhysicalDeviceOpenGL> GraphicsDeviceOpenGL::GetPhysicalDeviceOpenGL()
	{
		return m_PhysicalDevice;
	}

	Ref<GraphicsPipeline> GraphicsDeviceOpenGL::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<GraphicsPipelineOpenGL>(description, this);
	}

	Ref<ComputePipeline> GraphicsDeviceOpenGL::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<ComputePipelineOpenGL>(description, this);
	}

	Ref<MeshletPipeline> GraphicsDeviceOpenGL::CreateMeshletPipeline(const MeshletPipelineDescription &description)
	{
		NX_VALIDATE(false, "Meshlet pipelines are not supported by OpenGL");
		return nullptr;
	}

	Ref<RayTracingPipeline> GraphicsDeviceOpenGL::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
	{
		NX_VALIDATE(false, "Ray tracing pipelines are not supported by OpenGL");
		return nullptr;
	}

	Ref<CommandList> GraphicsDeviceOpenGL::CreateCommandList(const CommandListDescription &spec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<CommandListOpenGL>(spec);
	}

	Ref<ResourceSet> GraphicsDeviceOpenGL::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<ResourceSetOpenGL>(pipeline);
	}

	Ref<Framebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(const FramebufferSpecification &spec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<FramebufferOpenGL>(spec, this);
	}

	Ref<Sampler> GraphicsDeviceOpenGL::CreateSampler(const SamplerDescription &spec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<SamplerOpenGL>(spec, this);
	}

	Ref<TimingQuery> GraphicsDeviceOpenGL::CreateTimingQuery()
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<TimingQueryOpenGL>();
	}

	Ref<DeviceBuffer> GraphicsDeviceOpenGL::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<DeviceBufferOpenGL>(desc, this);
	}

	Ref<IAccelerationStructure> GraphicsDeviceOpenGL::CreateAccelerationStructure(const AccelerationStructureDescription &desc)
	{
		return nullptr;
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
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<SwapchainOpenGL>(window, spec, this);
	}

	Ref<Fence> GraphicsDeviceOpenGL::CreateFence(const FenceDescription &desc)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<FenceOpenGL>(desc, this);
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
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceOpenGL> fence = std::dynamic_pointer_cast<FenceOpenGL>(fences[i]);
			fence->Reset();
		}
	}

	Ref<Texture> GraphicsDeviceOpenGL::CreateTexture(const TextureDescription &spec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
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
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.Finish(); });
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