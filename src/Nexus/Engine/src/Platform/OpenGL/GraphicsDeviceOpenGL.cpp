#if defined(NX_PLATFORM_OPENGL)

	#include "GraphicsDeviceOpenGL.hpp"

	#include "CommandListOpenGL.hpp"
	#include "CommandQueueOpenGL.hpp"
	#include "DeviceBufferOpenGL.hpp"
	#include "FenceOpenGL.hpp"
	#include "PipelineOpenGL.hpp"
	#include "ResourceSetOpenGL.hpp"
	#include "SamplerOpenGL.hpp"
	#include "ShaderModuleOpenGL.hpp"
	#include "SwapchainOpenGL.hpp"
	#include "TexelBufferOpenGL.hpp"
	#include "TextureOpenGL.hpp"
	#include "TextureViewOpenGL.hpp"
	#include "TimingQueryOpenGL.hpp"

	#if defined(WIN32)
		#include "Surface/WGL/SurfaceWGL.hpp"
	#elif defined(__linux__)
		#include "Surface/EGL/SurfaceEGL.hpp"
	#endif

	#include "glad/gl.h"

namespace Nexus::Graphics
{
	static void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
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
				// retrieve available extensions
				m_Extensions = GetSupportedExtensions(context);

				// retrieve API and graphics adapter name
				m_APIName	   = std::string("OpenGL - ") + std::string((const char *)context.GetString(GL_VERSION));
				m_RendererName = (const char *)context.GetString(GL_RENDERER);

			// enable debugging if available
	#if !defined(__EMSCRIPTEN__)
				if (enableDebug)
				{
					context.Enable(GL_DEBUG_OUTPUT);
					context.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

					context.DebugMessageCallback(glDebugCallback, nullptr);
				}
	#endif
				// set pixel alignment to the default globally
				context.PixelStorei(GL_PACK_ALIGNMENT, 4);
				context.PixelStorei(GL_UNPACK_ALIGNMENT, 4);
			});

		GetFeatures();
	}

	GraphicsDeviceOpenGL::~GraphicsDeviceOpenGL()
	{
	}

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceOpenGL::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	GL::IOffscreenContext *GraphicsDeviceOpenGL::GetOffscreenContext()
	{
		return m_PhysicalDevice->GetOffscreenContext();
	}

	Ref<IShaderModule> GraphicsDeviceOpenGL::CreateShaderModule(const ShaderModuleDescription &moduleDesc)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<ShaderModuleOpenGL>(moduleDesc, this);
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

	void GraphicsDeviceOpenGL::GetFeatures()
	{
		GL::IOffscreenContext *offscreenContext = m_PhysicalDevice->GetOffscreenContext();
		GL::SetCurrentContext(offscreenContext);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				m_Features.SupportsGeometryShaders	   = context.ARB_geometry_shader4 || context.EXT_geometry_shader == 1;
				m_Features.SupportsTesselationShaders  = context.ARB_tessellation_shader == 1;
				m_Features.SupportsComputeShaders	   = context.ARB_compute_shader == 1;
				m_Features.SupportsStorageBuffers	   = context.ARB_buffer_storage == 1;
				m_Features.SupportsMultiviewport	   = context.OVR_multiview == 1;
				m_Features.SupportsSamplerAnisotropy   = context.ARB_texture_filter_anisotropic == 1 || context.EXT_texture_filter_anisotropic == 1;
				m_Features.SupportsETC2Compression	   = context.ARB_ES3_compatibility == 1;
				m_Features.SupportsASTC_LDRCompression = context.KHR_texture_compression_astc_ldr == 1;
				m_Features.SupportsBCCompression	   = context.EXT_texture_compression_s3tc == 1 || context.ARB_texture_compression_bptc == 1;

				// This may need revisiting
				m_Features.SupportShaderStorageImageMultisample = context.ARB_shader_image_load_store == 1;

				m_Features.SupportsCubemapArray		  = context.ARB_texture_cube_map_array == 1 || context.EXT_texture_cube_map_array == 1;
				m_Features.SupportsIndependentBlend	  = context.ARB_draw_buffers_blend == 1 || context.EXT_draw_buffers_indexed == 1;
				m_Features.SupportsMeshTaskShaders	  = context.EXT_mesh_shader == 1;
				m_Features.SupportsDepthBoundsTesting = context.EXT_depth_bounds_test == 1;
			});
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
		const AccelerationStructureGeometryBuildDescription &description) const
	{
		NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
		return AccelerationStructureBuildSizeDescription();
	}

	RayTracingDeviceDescription GraphicsDeviceOpenGL::GetRayTracingDeviceDescription() const
	{
		NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
		return RayTracingDeviceDescription();
	}

	AccelerationStructureProperties GraphicsDeviceOpenGL::GetAccelerationStructureProperties() const
	{
		NX_VALIDATE(0, "Ray tracing not supported on OpenGL backend");
		return AccelerationStructureProperties();
	}

	Ref<ISurface> GraphicsDeviceOpenGL::CreateSurfaceFromWin32(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance) const
	{
	#if defined(WIN32)
		return CreateRef<SurfaceWGL>(hwnd, hdc, hinstance, this);
	#else
		return nullptr;
	#endif
	}

	Ref<ISurface> GraphicsDeviceOpenGL::CreateSurfaceFromX11(uintptr_t display, uint32_t screen, uint32_t window) const
	{
	#if defined(__linux__)
		return CreateRef<SurfaceEGL>(display, screen, window, this);
	#else
		return nullptr;
	#endif
	}

	Ref<ISurface> GraphicsDeviceOpenGL::CreateSurfaceFromWayland(uintptr_t display, uintptr_t surface) const
	{
		return nullptr;
	}

	Ref<ISurface> GraphicsDeviceOpenGL::CreateSurfaceFromAndroid(uintptr_t nativeWindow) const
	{
		return nullptr;
	}

	Ref<ISurface> GraphicsDeviceOpenGL::CreateSurfaceFromHTML(const std::string &canvasId) const
	{
		return nullptr;
	}

	Ref<PhysicalDeviceOpenGL> GraphicsDeviceOpenGL::GetPhysicalDeviceOpenGL()
	{
		return m_PhysicalDevice;
	}

	Ref<IGraphicsPipeline> GraphicsDeviceOpenGL::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<GraphicsPipelineOpenGL>(description, this);
	}

	Ref<IComputePipeline> GraphicsDeviceOpenGL::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<ComputePipelineOpenGL>(description, this);
	}

	Ref<IMeshletPipeline> GraphicsDeviceOpenGL::CreateMeshletPipeline(const MeshletPipelineDescription &description)
	{
		NX_VALIDATE(false, "Meshlet pipelines are not supported by OpenGL");
		return nullptr;
	}

	Ref<IRayTracingPipeline> GraphicsDeviceOpenGL::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
	{
		NX_VALIDATE(false, "Ray tracing pipelines are not supported by OpenGL");
		return nullptr;
	}

	Ref<IResourceSet> GraphicsDeviceOpenGL::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<ResourceSetOpenGL>(pipeline, this);
	}

	Ref<IFramebuffer> GraphicsDeviceOpenGL::CreateFramebuffer(const FramebufferTextureSetDescription &desc)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<FramebufferOpenGL>(desc, this);
	}

	SamplerHandle GraphicsDeviceOpenGL::CreateSampler(const SamplerDescription &spec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		auto sampler = std::unique_ptr<ISampler>(new SamplerOpenGL(spec, this));
		return m_Resources.Samplers.CreateShared(std::move(sampler));
	}

	Ref<ITimingQuery> GraphicsDeviceOpenGL::CreateTimingQuery()
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<TimingQueryOpenGL>();
	}

	Ref<IDeviceBuffer> GraphicsDeviceOpenGL::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<DeviceBufferOpenGL>(desc, this);
	}

	Ref<IAccelerationStructure> GraphicsDeviceOpenGL::CreateAccelerationStructure(const AccelerationStructureDescription &desc)
	{
		return nullptr;
	}

	Ref<ITexelBuffer> GraphicsDeviceOpenGL::CreateTexelBuffer(const TexelBufferDescription &desc)
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

	Ref<IFence> GraphicsDeviceOpenGL::CreateFence(const FenceDescription &desc)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<FenceOpenGL>(desc, this);
	}

	FenceWaitResult GraphicsDeviceOpenGL::WaitForFences(Ref<IFence> *fences, uint32_t count, bool waitAll, uint64_t timeoutNS)
	{
		std::vector<FenceWaitResult> success(count);

		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceOpenGL> fence = std::dynamic_pointer_cast<FenceOpenGL>(fences[i]);

			GLenum result = fence->Wait(timeoutNS);
			if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED)
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

	std::vector<QueueFamilyInfo> GraphicsDeviceOpenGL::GetQueueFamilies()
	{
		std::vector<QueueFamilyInfo> queueFamilies = {};

		QueueFamilyInfo &info = queueFamilies.emplace_back();
		info.QueueFamily	  = 0;
		info.QueueCount		  = std::numeric_limits<uint32_t>::max();
		info.Capabilities	  = QueueCapabilities(QueueCapabilities::Graphics | QueueCapabilities::Compute | QueueCapabilities::Transfer);

		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_sparse_buffer && context.ARB_sparse_texture)
				{
					info.Capabilities = QueueCapabilities(info.Capabilities | QueueCapabilities::SparseBinding);
				}
			});

		return queueFamilies;
	}

	Ref<ICommandQueue> GraphicsDeviceOpenGL::CreateCommandQueue(const CommandQueueDescription &description)
	{
		return CreateRef<CommandQueueOpenGL>(this, description);
	}

	void GraphicsDeviceOpenGL::ResetFences(Ref<IFence> *fences, uint32_t count)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceOpenGL> fence = std::dynamic_pointer_cast<FenceOpenGL>(fences[i]);
			fence->Reset();
		}
	}

	Ref<ITexture> GraphicsDeviceOpenGL::CreateTexture(const TextureDescription &spec)
	{
		GL::SetCurrentContext(m_PhysicalDevice->GetOffscreenContext());
		return CreateRef<TextureOpenGL>(spec, this);
	}

	TextureViewHandle GraphicsDeviceOpenGL::CreateTextureView(const TextureViewDescription &desc)
	{
		Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(desc.TargetTexture);

		auto textureView = std::make_unique<TextureViewOpenGL>(desc, this);
		return m_Resources.TextureViews.CreateShared(std::move(textureView));
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
	}

	GraphicsAPIInfo GraphicsDeviceOpenGL::GetGraphicsAPI()
	{
		GLint major = 0;
		GLint minor = 0;

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				context.GetIntegerv(GL_MAJOR_VERSION, &major);
				context.GetIntegerv(GL_MINOR_VERSION, &minor);
			});

		return GraphicsAPIInfo {
			.API   = GraphicsAPI::OpenGL,
			.Major = static_cast<uint32_t>(major),
			.Minor = static_cast<uint32_t>(minor),
		};
	}

	bool GraphicsDeviceOpenGL::Validate()
	{
		GL::IOffscreenContext *context = m_PhysicalDevice->GetOffscreenContext();
		return context->Validate();
	}
}	 // namespace Nexus::Graphics

#endif