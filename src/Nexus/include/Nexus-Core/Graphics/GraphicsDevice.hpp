#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "CommandList.hpp"
#include "Framebuffer.hpp"
#include "GPUBuffer.hpp"
#include "GraphicsCapabilities.hpp"
#include "Nexus-Core/Graphics/ShaderGenerator.hpp"
#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/Types.hpp"
#include "Pipeline.hpp"
#include "ResourceSet.hpp"
#include "Sampler.hpp"
#include "ShaderModule.hpp"
#include "Swapchain.hpp"
#include "Texture.hpp"
#include "TimingQuery.hpp"
#include "Viewport.hpp"

#include "Nexus-Core/IResource.hpp"

namespace Nexus::Graphics
{
	/// @brief A class representing properties needed to create a new graphics
	/// device
	struct GraphicsDeviceSpecification
	{
		/// @brief The chosen graphics API to use to create the GraphicsDevice with
		GraphicsAPI API;

		/// @brief Debugging will be enabled for the native graphics API
		bool DebugLayer = false;
	};

	/// @brief A class representing an abstraction over a graphics API
	class GraphicsDevice : public IResource
	{
	  public:
		NX_API static GraphicsDevice *CreateGraphicsDevice(const Graphics::GraphicsDeviceSpecification &spec);

		/// @brief A constructor taking in a const reference to a
		/// GraphicsDeviceSpecification
		/// @param createInfo The options to use when creating the GraphicsDevice
		NX_API GraphicsDevice(const GraphicsDeviceSpecification &createInfo);

		/// @brief A virtual destructor allowing resources to be deleted
		NX_API virtual ~GraphicsDevice()
		{
	  }

		/// @brief Copying a GraphicsDevice is not supported
		/// @param Another GraphicsDevice taken by const reference
		NX_API GraphicsDevice(const GraphicsDevice &) = delete;

		/// @brief A pure virtual method that returns the name of the graphics API as
		/// a string
		/// @return A string containing the API name
		NX_API virtual const std::string GetAPIName() = 0;

		/// @brief A pure virtual method that will return the name of the device
		/// currently being used to render graphics
		/// @return A const char* containing the device name
		NX_API virtual const char *GetDeviceName() = 0;

		/// @brief A pure virtual method that will submit a command list for rendering
		/// @param commandList The command list to submit for rendering
		NX_API virtual void SubmitCommandList(Ref<CommandList> commandList) = 0;

		/// @brief A method that returns an enum value representing the currently
		/// running graphics API backend
		/// @return A GraphicsAPI enum containing the current backend
		NX_API GraphicsAPI GetGraphicsAPI()
		{
			return m_Specification.API;
		}

		/// @brief A pure virtual method that creates a pipeline from a given pipeline
		/// description
		/// @param description The properties to use when creating the pipeline
		/// @return A pointer to a pipeline
		NX_API virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) = 0;

		/// @brief A pure virtual method that creates a vertex buffer from a given
		/// description
		/// @param description The properties to use when creating the buffer
		/// @param data The initial data to store in the buffer
		/// @param layout The layout of the vertex buffer
		/// @return A pointer to a vertex buffer
		NX_API virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data) = 0;

		/// @brief A pure virtual method that creates an index buffer from a given
		/// description
		/// @param description The properties to use when creating the buffer
		/// @param data The initial data to store in the buffer
		/// @return A pointer to an index buffer
		NX_API virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description,
														  const void			  *data,
														  IndexBufferFormat		   format = IndexBufferFormat::UInt32) = 0;

		/// @brief A pure virtual method that creates a uniform buffer from a given
		/// description
		/// @param description The properties to use when creating the buffer
		/// @param data The initial data to store in the buffer
		/// @return A pointer to a uniform buffer
		NX_API virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) = 0;

		/// @brief A pure virtual method that creates a new command list
		/// @return A pointer to a command list
		NX_API virtual Ref<CommandList> CreateCommandList(const CommandListSpecification &spec = {}) = 0;

		/// @brief A pure virtual method that creates a new texture from a given
		/// specification
		/// @param spec The properties to use when creating the texture
		/// @return A pointer to a texture
		NX_API virtual Ref<Texture2D> CreateTexture2D(const Texture2DSpecification &spec) = 0;

		/// @brief A method that loads a new texture from a image stored on disk
		/// @param filepath The filepath to load the image from
		/// @return A pointer to a texture
		NX_API Ref<Texture2D> CreateTexture2D(const char *filepath, bool generateMips);

		/// @brief A method that loads a new texture from an image stored on disk
		/// @param filepath The filepath to load the image from
		/// @return A pointer to a texture
		NX_API Ref<Texture2D> CreateTexture2D(const std::string &filepath, bool generateMips);

		NX_API virtual Ref<Cubemap> CreateCubemap(const CubemapSpecification &spec) = 0;

		NX_API virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) = 0;

		/// @brief A pure virtual method that creates a new resource set from a given
		/// specification
		/// @param spec A set of properties to use when creating the resource set
		/// @return A pointer to a resource set
		NX_API virtual Ref<ResourceSet> CreateResourceSet(const ResourceSetSpecification &spec) = 0;

		/// @brief A method that creates a new resource set from a pipeline
		/// @param pipeline A pipeline to use when creating the resource set
		/// @return A pointer to a resource set
		NX_API Ref<ResourceSet> CreateResourceSet(Ref<Pipeline> pipeline);

		/// @brief A pure virtual method that creates a new sampler from a given
		/// specification
		/// @param spec A set of properties to use when creating the sampler
		/// @return A pointer to a sampler
		NX_API virtual Ref<Sampler> CreateSampler(const SamplerSpecification &spec) = 0;

		NX_API virtual Ref<TimingQuery> CreateTimingQuery() = 0;

		/// @brief A pure virtual method that returns a ShaderFormat enum representing
		/// the supported shading language of the backend
		/// @return The supported shading language of the backend
		NX_API virtual ShaderLanguage GetSupportedShaderFormat() = 0;

		NX_API virtual void WaitForIdle() = 0;

		/// @brief A pure virtual method that returns a value that can be used to
		/// standardise UV coordinates across backends
		/// @return A float representing the correction
		NX_API virtual float GetUVCorrection() = 0;

		NX_API virtual bool IsUVOriginTopLeft() = 0;

		NX_API virtual const GraphicsCapabilities GetGraphicsCapabilities() const = 0;

		NX_API virtual Swapchain *CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) = 0;

		NX_API Ref<ShaderModule> CreateShaderModuleFromSpirvFile(const std::string &filepath, ShaderStage stage);

		NX_API Ref<ShaderModule> CreateShaderModuleFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

		NX_API Ref<ShaderModule> GetOrCreateCachedShaderFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

		NX_API Ref<ShaderModule> GetOrCreateCachedShaderFromSpirvFile(const std::string &filepath, ShaderStage stage);

		NX_API void ImmediateSubmit(std::function<void(Ref<CommandList> cmd)> &&function);

		NX_API const GraphicsDeviceSpecification &GetSpecification() const;

		NX_API virtual bool				  Validate() override;
		NX_API virtual void				  SetName(const std::string &name) override;
		NX_API virtual const std::string &GetName() override;

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) = 0;
		Ref<ShaderModule>		  TryLoadCachedShader(const std::string &source, const std::string &name, ShaderStage stage, ShaderLanguage language);

	  protected:
		GraphicsDeviceSpecification m_Specification;

		Ref<CommandList> m_ImmediateCommandList = nullptr;
		std::string		 m_Name					= "GraphicsDevice";
	};
}	 // namespace Nexus::Graphics