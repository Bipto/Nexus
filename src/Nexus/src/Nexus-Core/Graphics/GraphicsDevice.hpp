#pragma once

#include "CommandList.hpp"
#include "Framebuffer.hpp"
#include "GPUBuffer.hpp"
#include "GraphicsAPI.hpp"
#include "GraphicsCapabilities.hpp"
#include "Nexus-Core/Graphics/ShaderGenerator.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/Window.hpp"
#include "Pipeline.hpp"
#include "ResourceSet.hpp"
#include "SDL.h"
#include "Sampler.hpp"
#include "ShaderModule.hpp"
#include "Swapchain.hpp"
#include "Texture.hpp"
#include "TimingQuery.hpp"
#include "Viewport.hpp"

namespace Nexus::Graphics
{
/// @brief A class representing properties needed to create a new graphics device
struct GraphicsDeviceSpecification
{
    /// @brief The chosen graphics API to use to create the GraphicsDevice with
    GraphicsAPI API;

    /// @brief Debugging will be enabled for the native graphics API
    bool DebugLayer = false;
};

/// @brief A class representing an abstraction over a graphics API
class GraphicsDevice
{
  public:
    /// @brief A constructor taking in a const reference to a GraphicsDeviceSpecification
    /// @param createInfo The options to use when creating the GraphicsDevice
    GraphicsDevice(const GraphicsDeviceSpecification &createInfo, Window *window, const SwapchainSpecification &swapchainSpec);

    /// @brief A virtual destructor allowing resources to be deleted
    virtual ~GraphicsDevice()
    {
    }

    /// @brief Copying a GraphicsDevice is not supported
    /// @param Another GraphicsDevice taken by const reference
    GraphicsDevice(const GraphicsDevice &) = delete;

    /// @brief A pure virtual method that returns the name of the graphics API as a string
    /// @return A string containing the API name
    virtual const std::string GetAPIName() = 0;

    /// @brief A pure virtual method that will return the name of the device currently being used to render graphics
    /// @return A const char* containing the device name
    virtual const char *GetDeviceName() = 0;

    /// @brief A pure virtual method that will submit a command list for rendering
    /// @param commandList The command list to submit for rendering
    virtual void SubmitCommandList(Ref<CommandList> commandList) = 0;

    /// @brief A method that returns an enum value representing the currently running graphics API backend
    /// @return A GraphicsAPI enum containing the current backend
    GraphicsAPI GetGraphicsAPI()
    {
        return m_Specification.API;
    }

    /// @brief A pure virtual method that creates a pipeline from a given pipeline description
    /// @param description The properties to use when creating the pipeline
    /// @return A pointer to a pipeline
    virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) = 0;

    /// @brief A pure virtual method that creates a vertex buffer from a given description
    /// @param description The properties to use when creating the buffer
    /// @param data The initial data to store in the buffer
    /// @param layout The layout of the vertex buffer
    /// @return A pointer to a vertex buffer
    virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data) = 0;

    /// @brief A pure virtual method that creates an index buffer from a given description
    /// @param description The properties to use when creating the buffer
    /// @param data The initial data to store in the buffer
    /// @return A pointer to an index buffer
    virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) = 0;

    /// @brief A pure virtual method that creates a uniform buffer from a given description
    /// @param description The properties to use when creating the buffer
    /// @param data The initial data to store in the buffer
    /// @return A pointer to a uniform buffer
    virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) = 0;

    /// @brief A pure virtual method that creates a new command list
    /// @return A pointer to a command list
    virtual Ref<CommandList> CreateCommandList(const CommandListSpecification &spec = {}) = 0;

    /// @brief A pure virtual method that creates a new texture from a given specification
    /// @param spec The properties to use when creating the texture
    /// @return A pointer to a texture
    virtual Ref<Texture> CreateTexture(const TextureSpecification &spec) = 0;

    /// @brief A method that loads a new texture from a image stored on disk
    /// @param filepath The filepath to load the image from
    /// @return A pointer to a texture
    Ref<Texture> CreateTexture(const char *filepath, bool generateMips);

    /// @brief A method that loads a new texture from an image stored on disk
    /// @param filepath The filepath to load the image from
    /// @return A pointer to a texture
    Ref<Texture> CreateTexture(const std::string &filepath, bool generateMips);

    virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) = 0;

    /// @brief A pure virtual method that creates a new resource set from a given specification
    /// @param spec A set of properties to use when creating the resource set
    /// @return A pointer to a resource set
    virtual Ref<ResourceSet> CreateResourceSet(const ResourceSetSpecification &spec) = 0;

    /// @brief A method that creates a new resource set from a pipeline
    /// @param pipeline A pipeline to use when creating the resource set
    /// @return A pointer to a resource set
    Ref<ResourceSet> CreateResourceSet(Ref<Pipeline> pipeline);

    /// @brief A pure virtual method that creates a new sampler from a given specification
    /// @param spec A set of properties to use when creating the sampler
    /// @return A pointer to a sampler
    virtual Ref<Sampler> CreateSampler(const SamplerSpecification &spec) = 0;

    virtual Ref<TimingQuery> CreateTimingQuery() = 0;

    /// @brief A pure virtual method that returns a ShaderFormat enum representing the supported shading language of the backend
    /// @return The supported shading language of the backend
    virtual ShaderLanguage GetSupportedShaderFormat() = 0;

    /// @brief A pure virtual method that returns a value that can be used to standardise UV coordinates across backends
    /// @return A float representing the correction
    virtual float GetUVCorrection() = 0;

    virtual bool IsUVOriginTopLeft() = 0;

    virtual const GraphicsCapabilities GetGraphicsCapabilities() const = 0;

    Ref<ShaderModule> CreateShaderModuleFromSpirvFile(const std::string &filepath, ShaderStage stage);

    Ref<ShaderModule> CreateShaderModuleFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage);

    Window *GetPrimaryWindow();

    void ImmediateSubmit(std::function<void(Ref<CommandList> cmd)> &&function);

    const GraphicsDeviceSpecification &GetSpecification() const;

  private:
    virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) = 0;

  protected:
    /// @brief A pointer to the window to render graphics into
    Nexus::Window *m_Window = nullptr;

    GraphicsDeviceSpecification m_Specification;

    Ref<CommandList> m_ImmediateCommandList = nullptr;
};
} // namespace Nexus::Graphics