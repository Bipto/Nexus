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
#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/Types.hpp"
#include "Pipeline.hpp"
#include "PixelFormatProperties.hpp"
#include "ResourceSet.hpp"
#include "Sampler.hpp"
#include "ShaderModule.hpp"
#include "Swapchain.hpp"
#include "Texture.hpp"
#include "TimingQuery.hpp"
#include "Viewport.hpp"

#include "Nexus-Core/Graphics/GraphicsAPICreateInfo.hpp"

namespace Nexus::Graphics
{
	/// @brief A class representing an abstraction over a graphics API
	class NX_API GraphicsDevice
	{
	  public:
		GraphicsDevice() = default;

		/// @brief A virtual destructor allowing resources to be deleted
		virtual ~GraphicsDevice() = default;

		/// @brief Copying a GraphicsDevice is not supported
		/// @param Another GraphicsDevice taken by const reference
		GraphicsDevice(const GraphicsDevice &) = delete;

		/// @brief A pure virtual method that returns the name of the graphics API as
		/// a string
		/// @return A string containing the API name
		virtual const std::string GetAPIName() = 0;

		/// @brief A pure virtual method that creates a pipeline from a given pipeline
		/// description
		/// @param description The properties to use when creating the pipeline
		/// @return A pointer to a pipeline
		virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription &description) = 0;

		virtual Ref<ComputePipeline> CreateComputePipeline(const ComputePipelineDescription &description) = 0;

		virtual Ref<MeshletPipeline> CreateMeshletPipeline(const MeshletPipelineDescription &description) = 0;

		virtual Ref<RayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDescription &description) = 0;

		/// @brief A method that loads a new texture from a image stored on disk
		/// @param filepath The filepath to load the image from
		/// @return A pointer to a texture
		Ref<Texture> CreateTexture2D(Ref<ICommandQueue> commandQueue, const char *filepath, bool generateMips, bool srgb = false);

		/// @brief A method that loads a new texture from an image stored on disk
		/// @param filepath The filepath to load the image from
		/// @return A pointer to a texture
		Ref<Texture> CreateTexture2D(Ref<ICommandQueue> commandQueue, const std::string &filepath, bool generateMips, bool srgb = false);

		virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) = 0;

		/// @brief A pure virtual method that creates a new resource set from a given
		/// specification
		/// @param spec A set of properties to use when creating the resource set
		/// @return A pointer to a resource set
		virtual Ref<ResourceSet> CreateResourceSet(Ref<Pipeline> pipeline) = 0;

		/// @brief A pure virtual method that creates a new sampler from a given
		/// specification
		/// @param spec A set of properties to use when creating the sampler
		/// @return A pointer to a sampler
		virtual Ref<Sampler> CreateSampler(const SamplerDescription &spec) = 0;

		virtual Ref<DeviceBuffer> CreateDeviceBuffer(const DeviceBufferDescription &desc) = 0;

		virtual Ref<TimingQuery> CreateTimingQuery() = 0;

		virtual Ref<IAccelerationStructure> CreateAccelerationStructure(const AccelerationStructureDescription &desc) = 0;

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

		virtual Ref<Texture> CreateTexture(const TextureDescription &spec) = 0;

		virtual Ref<Fence> CreateFence(const FenceDescription &desc) = 0;

		virtual FenceWaitResult WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) = 0;

		virtual void ResetFences(Ref<Fence> *fences, uint32_t count) = 0;

		virtual std::vector<QueueFamilyInfo> GetQueueFamilies() = 0;

		virtual Ref<ICommandQueue> CreateCommandQueue(const CommandQueueDescription &description) = 0;

		Ref<ShaderModule> CreateShaderModuleFromSpirvFile(const std::string &filepath, ShaderStage stage);

		Ref<ShaderModule> CreateShaderModuleFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

		Ref<ShaderModule> GetOrCreateCachedShaderFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

		Ref<ShaderModule> GetOrCreateCachedShaderFromSpirvFile(const std::string &filepath, ShaderStage stage);

		void WriteToTexture(Ref<Texture>	   texture,
							Ref<ICommandQueue> commandQueue,
							uint32_t		   arrayLayer,
							uint32_t		   mipLevel,
							uint32_t		   x,
							uint32_t		   y,
							uint32_t		   z,
							uint32_t		   width,
							uint32_t		   height,
							const void		  *data,
							size_t			   size);

		std::vector<char> ReadFromTexture(Ref<Texture>		 texture,
										  Ref<ICommandQueue> commandQueue,
										  uint32_t			 arrayLayer,
										  uint32_t			 mipLevel,
										  uint32_t			 x,
										  uint32_t			 y,
										  uint32_t			 z,
										  uint32_t			 width,
										  uint32_t			 height);

		virtual bool							 Validate()				   = 0;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const = 0;

		virtual PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const = 0;

		virtual const DeviceFeatures					 &GetPhysicalDeviceFeatures() const						 = 0;
		virtual const DeviceLimits						 &GetPhysicalDeviceLimits() const						 = 0;
		virtual bool									  IsIndexBufferFormatSupported(IndexFormat format) const = 0;
		virtual AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(
			const AccelerationStructureGeometryBuildDescription &description,
			const std::vector<uint32_t>							&primitiveCount) const = 0;

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec) = 0;
		Ref<ShaderModule>		  TryLoadCachedShader(const std::string &source, const std::string &name, ShaderStage stage, ShaderLanguage language);

	  protected:
		Ref<CommandList> m_ImmediateCommandList = nullptr;
	};
}	 // namespace Nexus::Graphics