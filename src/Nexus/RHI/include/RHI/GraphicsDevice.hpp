#pragma once
#include "RHI/AccelerationStructure.hpp"
#include "RHI/CommandList.hpp"
#include "RHI/CommandQueue.hpp"
#include "RHI/DeviceAddress.hpp"
#include "RHI/DeviceBuffer.hpp"
#include "RHI/Fence.hpp"
#include "RHI/Framebuffer.hpp"
#include "RHI/GraphicsCapabilities.hpp"
#include "RHI/IPhysicalDevice.hpp"
#include "RHI/ISurface.hpp"
#include "RHI/IndirectDrawArguments.hpp"
#include "RHI/Pipeline.hpp"
#include "RHI/PixelFormatProperties.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/ResourceSet.hpp"
#include "RHI/Sampler.hpp"
#include "RHI/ShaderLanguage.hpp"
#include "RHI/ShaderModule.hpp"
#include "RHI/Swapchain.hpp"
#include "RHI/TexelBuffer.hpp"
#include "RHI/Texture.hpp"
#include "RHI/TextureView.hpp"
#include "RHI/TimingQuery.hpp"
#include "RHI/Types.hpp"
#include "RHI/Viewport.hpp"

#include "RHI/GraphicsAPICreateInfo.hpp"

namespace Nexus::Graphics
{
	struct AccelerationStructureProperties
	{
		uint64_t MaxGeometryCount								= 0;
		uint64_t MaxInstanceCount								= 0;
		uint64_t MaxPrimitiveCount								= 0;
		uint32_t MinAccelerationStructureScratchOffsetAlignment = 0;
	};

	struct RayTracingDeviceDescription
	{
		uint32_t ShaderGroupHandleSize				= 0;
		uint32_t MaxRayRecursionDepth				= 0;
		uint32_t MaxShaderGroupStride				= 0;
		uint32_t ShaderGroupBaseAlignment			= 0;
		uint32_t ShaderGroupHandleCaptureReplaySize = 0;
		uint32_t MaxRayDispatchInvocationCount		= 0;
		uint32_t ShaderGroupHandleAlignment			= 0;
		uint32_t MaxRayHitAttributeSize				= 0;
	};

	/// @brief A class representing an abstraction over a graphics API
	class NX_RHI_API IGraphicsDevice
	{
	  public:
		IGraphicsDevice() = default;

		/// @brief A virtual destructor allowing resources to be deleted
		virtual ~IGraphicsDevice() = default;

		/// @brief Copying a GraphicsDevice is not supported
		/// @param Another GraphicsDevice taken by const reference
		IGraphicsDevice(const IGraphicsDevice &) = delete;

		/// @brief A pure virtual method that returns the name of the graphics API as
		/// a string
		/// @return A string containing the API name
		virtual const std::string GetAPIName() = 0;

		/// @brief A pure virtual method that creates a pipeline from a given pipeline
		/// description
		/// @param description The properties to use when creating the pipeline
		/// @return A pointer to a pipeline
		virtual Ref<IGraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription &description) = 0;

		virtual Ref<IComputePipeline> CreateComputePipeline(const ComputePipelineDescription &description) = 0;

		virtual Ref<IMeshletPipeline> CreateMeshletPipeline(const MeshletPipelineDescription &description) = 0;

		virtual Ref<IRayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDescription &description) = 0;

		virtual Ref<IFramebuffer> CreateFramebuffer(const FramebufferTextureSetDescription &desc) = 0;

		/// @brief A pure virtual method that creates a new resource set from a given
		/// specification
		/// @param spec A set of properties to use when creating the resource set
		/// @return A pointer to a resource set
		virtual Ref<IResourceSet> CreateResourceSet(Ref<Pipeline> pipeline) = 0;

		/// @brief A pure virtual method that creates a new sampler from a given
		/// specification
		/// @param spec A set of properties to use when creating the sampler
		/// @return A pointer to a sampler
		virtual Ref<ISampler> CreateSampler(const SamplerDescription &spec) = 0;

		virtual Ref<IDeviceBuffer> CreateDeviceBuffer(const DeviceBufferDescription &desc) = 0;

		virtual Ref<ITimingQuery> CreateTimingQuery() = 0;

		virtual Ref<IAccelerationStructure> CreateAccelerationStructure(const AccelerationStructureDescription &desc) = 0;

		virtual Ref<ITexelBuffer> CreateTexelBuffer(const TexelBufferDescription &desc) = 0;

		/// @brief A pure virtual method that returns a ShaderFormat enum representing
		/// the supported shading language of the backend
		/// @return The supported shading language of the backend
		virtual ShaderLanguage GetSupportedShaderFormat() = 0;

		virtual bool IsBufferUsageSupported(BufferUsage usage) = 0;

		virtual void WaitForIdle() = 0;

		/// @brief A pure virtual method that returns a value that can be used to
		/// standardise UV coordinates across backends
		/// @return A float representing the correction
		virtual float GetUVCorrection() = 0;

		virtual bool IsUVOriginTopLeft() = 0;

		virtual GraphicsAPI GetGraphicsAPI() = 0;

		virtual const GraphicsCapabilities GetGraphicsCapabilities() const = 0;

		virtual Ref<ITexture> CreateTexture(const TextureDescription &spec) = 0;

		virtual Ref<ITextureView> CreateTextureView(const TextureViewDescription &desc) = 0;

		virtual Ref<IFence> CreateFence(const FenceDescription &desc) = 0;

		virtual FenceWaitResult WaitForFences(Ref<IFence> *fences, uint32_t count, bool waitAll, uint64_t timeoutNS) = 0;

		virtual Ref<IShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec) = 0;

		virtual void ResetFences(Ref<IFence> *fences, uint32_t count) = 0;

		virtual std::vector<QueueFamilyInfo> GetQueueFamilies() = 0;

		virtual Ref<ICommandQueue> CreateCommandQueue(const CommandQueueDescription &description) = 0;

		virtual bool							 Validate()				   = 0;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const = 0;

		virtual PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const = 0;

		virtual const DeviceFeatures					 &GetPhysicalDeviceFeatures() const						 = 0;
		virtual const DeviceLimits						 &GetPhysicalDeviceLimits() const						 = 0;
		virtual bool									  IsIndexBufferFormatSupported(IndexFormat format) const = 0;
		virtual AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(
			const AccelerationStructureGeometryBuildDescription &description) const = 0;

		virtual RayTracingDeviceDescription		GetRayTracingDeviceDescription() const	   = 0;
		virtual AccelerationStructureProperties GetAccelerationStructureProperties() const = 0;

		virtual Ref<ISurface> CreateSurfaceFromWin32(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance) const = 0;
		virtual Ref<ISurface> CreateSurfaceFromX11(uintptr_t display, uint32_t screen, uint32_t window) const  = 0;
		virtual Ref<ISurface> CreateSurfaceFromWayland(uintptr_t display, uintptr_t surface) const			   = 0;
		virtual Ref<ISurface> CreateSurfaceFromAndroid(uintptr_t nativeWindow) const						   = 0;
		virtual Ref<ISurface> CreateSurfaceFromHTML(const std::string &canvasId) const						   = 0;

	  protected:
		Ref<ICommandList> m_ImmediateCommandList = nullptr;
	};
}	 // namespace Nexus::Graphics