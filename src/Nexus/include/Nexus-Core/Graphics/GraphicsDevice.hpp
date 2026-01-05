#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "AccelerationStructure.hpp"
#include "CommandList.hpp"
#include "CommandQueue.hpp"
#include "DeviceBuffer.hpp"
#include "Fence.hpp"
#include "Framebuffer.hpp"
#include "GraphicsCapabilities.hpp"
#include "IPhysicalDevice.hpp"
#include "IndirectDrawArguments.hpp"
#include "Nexus-Core/Graphics/ShaderGenerator.hpp"
#include "Nexus-Core/Types.hpp"
#include "Pipeline.hpp"
#include "PixelFormatProperties.hpp"
#include "Platform/IWindow.hpp"
#include "ResourceSet.hpp"
#include "Sampler.hpp"
#include "ShaderModule.hpp"
#include "Swapchain.hpp"
#include "TexelBuffer.hpp"
#include "Texture.hpp"
#include "TextureView.hpp"
#include "TimingQuery.hpp"
#include "Viewport.hpp"

#include "DeviceAddress.hpp"

#include "Nexus-Core/Graphics/GraphicsAPICreateInfo.hpp"

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
	class NX_API IGraphicsDevice
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

		/// @brief A method that loads a new texture from a image stored on disk
		/// @param filepath The filepath to load the image from
		/// @return A pointer to a texture
		Ref<ITexture> CreateTexture2D(Ref<ICommandQueue> commandQueue, const char *filepath, bool generateMips, bool srgb = false);

		/// @brief A method that loads a new texture from an image stored on disk
		/// @param filepath The filepath to load the image from
		/// @return A pointer to a texture
		Ref<ITexture> CreateTexture2D(Ref<ICommandQueue> commandQueue, const std::string &filepath, bool generateMips, bool srgb = false);

		std::pair<Ref<ITexture>, Ref<ITextureView>> CreateTexture2DWithView(Ref<ICommandQueue> commandQueue,
																			const char		  *filepath,
																			bool			   generateMips,
																			bool			   srgb = false);

		std::pair<Ref<ITexture>, Ref<ITextureView>> CreateTexture2DWithView(Ref<ICommandQueue> commandQueue,
																			const std::string &filepath,
																			bool			   generateMips,
																			bool			   srgb = false);

		virtual Ref<IFramebuffer> CreateFramebuffer(const FramebufferTextureSetDescription &desc) = 0;

		Ref<IFramebuffer> CreateFramebuffer(const FramebufferTextureCreateDescription &desc);

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

		virtual FenceWaitResult WaitForFences(Ref<IFence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) = 0;

		virtual void ResetFences(Ref<IFence> *fences, uint32_t count) = 0;

		virtual std::vector<QueueFamilyInfo> GetQueueFamilies() = 0;

		virtual Ref<ICommandQueue> CreateCommandQueue(const CommandQueueDescription &description) = 0;

		Ref<IShaderModule> CreateShaderModuleFromSpirvFile(const std::string &filepath, ShaderStage stage);

		Ref<IShaderModule> CreateShaderModuleFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

		Ref<IShaderModule> GetOrCreateCachedShaderFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

		Ref<IShaderModule> GetOrCreateCachedShaderFromSpirvFile(const std::string &filepath, ShaderStage stage);

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

	  private:
		virtual Ref<IShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec) = 0;
		Ref<IShaderModule> TryLoadCachedShader(const std::string &source, const std::string &name, ShaderStage stage, ShaderLanguage language);

	  protected:
		Ref<ICommandList> m_ImmediateCommandList = nullptr;
	};
}	 // namespace Nexus::Graphics