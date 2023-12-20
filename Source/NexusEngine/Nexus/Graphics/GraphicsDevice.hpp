#pragma once

#include "Nexus/Window.hpp"
#include "Nexus/Types.hpp"
#include "SDL.h"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Pipeline.hpp"
#include "CommandList.hpp"
#include "Nexus/Graphics/ShaderGenerator.hpp"
#include "Viewport.hpp"
#include "GraphicsAPI.hpp"
#include "Swapchain.hpp"
#include "ResourceSet.hpp"

namespace Nexus::Graphics
{
    /// @brief A class representing properties needed to create a new graphics device
    struct GraphicsDeviceCreateInfo
    {
        /// @brief The chosen graphics API to use to create the GraphicsDevice with
        GraphicsAPI API;
    };

    /// @brief A class representing an abstraction over a graphics API
    class GraphicsDevice
    {
    public:
        /// @brief A constructor taking in a const reference to a GraphicsDeviceCreateInfo
        /// @param createInfo The options to use when creating the GraphicsDevice
        GraphicsDevice(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec);

        /// @brief A virtual destructor allowing resources to be deleted
        virtual ~GraphicsDevice() {}

        /// @brief Copying a GraphicsDevice is not supported
        /// @param Another GraphicsDevice taken by const reference
        GraphicsDevice(const GraphicsDevice &) = delete;

        /// @brief A pure virtual method that sets makes the current graphics context current
        virtual void SetContext() = 0;

        /// @brief A pure virtual method that returns the name of the graphics API as a string
        /// @return A string containing the API name
        virtual const std::string GetAPIName() = 0;

        /// @brief A pure virtual method that will return the name of the device currently being used to render graphics
        /// @return A const char* containing the device name
        virtual const char *GetDeviceName() = 0;

        /// @brief A pure virtual method that will submit a command list for rendering
        /// @param commandList The command list to submit for rendering
        virtual void SubmitCommandList(CommandList *commandList) = 0;

        /// @brief A pure virtual method that returns a context for a graphics API
        /// @return A void pointer to the graphics context
        virtual void *GetContext() = 0;

        /// @brief A pure virtual method that will execute instructions to begin a frame
        virtual void BeginFrame() = 0;

        /// @brief A pure virtual method that will execute instructions to end a frame
        virtual void EndFrame() = 0;

        /// @brief A method that returns an enum value representing the currently running graphics API backend
        /// @return A GraphicsAPI enum containing the current backend
        GraphicsAPI GetGraphicsAPI() { return this->m_API; }

        /// @brief A pure virtual method that creates a new shader from a vertex and fragment shader and a vertex buffer layout
        /// @param vertexShaderSource A vertex shader written in the native shading language of the backend
        /// @param fragmentShaderSource A fragment shader written in the native shading language of the backend
        /// @param layout A parameter specifying how data is laid out in the vertex buffer
        /// @return A pointer to a shader
        virtual Shader *CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) = 0;

        /// @brief A pure virtual method that creates a pipeline from a given pipeline description
        /// @param description The properties to use when creating the pipeline
        /// @return A pointer to a pipeline
        virtual Pipeline *CreatePipeline(const PipelineDescription &description) = 0;

        /// @brief A pure virtual method that creates a vertex buffer from a given description
        /// @param description The properties to use when creating the buffer
        /// @param data The initial data to store in the buffer
        /// @param layout The layout of the vertex buffer
        /// @return A pointer to a vertex buffer
        virtual VertexBuffer *CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) = 0;

        /// @brief A pure virtual method that creates an index buffer from a given description
        /// @param description The properties to use when creating the buffer
        /// @param data The initial data to store in the buffer
        /// @return A pointer to an index buffer
        virtual IndexBuffer *CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) = 0;

        /// @brief A pure virtual method that creates a uniform buffer from a given description
        /// @param description The properties to use when creating the buffer
        /// @param data The initial data to store in the buffer
        /// @return A pointer to a uniform buffer
        virtual UniformBuffer *CreateUniformBuffer(const BufferDescription &description, const void *data) = 0;

        /// @brief A pure virtual method that creates a new command list
        /// @return A pointer to a command list
        virtual CommandList *CreateCommandList() = 0;

        /// @brief A pure virtual method that creates a new texture from a given specification
        /// @param spec The properties to use when creating the texture
        /// @return A pointer to a texture
        virtual Texture *CreateTexture(const TextureSpecification &spec) = 0;

        /// @brief A method that loads a new texture from a image stored on disk
        /// @param filepath The filepath to load the image from
        /// @return A pointer to a texture
        Texture *CreateTexture(const char *filepath);

        /// @brief A method that loads a new texture from an image stored on disk
        /// @param filepath The filepath to load the image from
        /// @return A pointer to a texture
        Texture *CreateTexture(const std::string &filepath);

        virtual Framebuffer *CreateFramebuffer(const FramebufferSpecification &spec) = 0;

        /// @brief A pure virtual method that creates a new resource set from a given specification
        /// @param spec A set of properties to use when creating the resource set
        /// @return A pointer to a resource set
        virtual ResourceSet *CreateResourceSet(const ResourceSetSpecification &spec) = 0;

        /// @brief A method that creates a new resource set from a pipeline
        /// @param pipeline A pipeline to use when creating the resource set
        /// @return A pointer to a resource set
        ResourceSet *CreateResourceSet(Pipeline *pipeline);

        /// @brief A pure virtual method that returns a ShaderFormat enum representing the supported shading language of the backend
        /// @return The supported shading language of the backend
        virtual ShaderLanguage GetSupportedShaderFormat() = 0;

        /// @brief A pure virtual method that returns a value that can be used to standardise UV coordinates across backends
        /// @return A float representing the correction
        virtual float GetUVCorrection() = 0;

        /// @brief A method that generates a supported shader type from a GLSL file and a vertex buffer layout
        /// @param filepath The filepath to load the GLSL shader from
        /// @param layout A vertex buffer layout to use to create the shader
        /// @return A pointer to a shader
        Shader *CreateShaderFromSpirvFile(const std::string &filepath, const VertexBufferLayout &layout);

        Window *GetPrimaryWindow();

    protected:
        /// @brief A pointer to the window to render graphics into
        Nexus::Window *m_Window;

        /// @brief A value representing the graphics API being used by the GraphicsDevice
        GraphicsAPI m_API;

        /// @brief The currently used viewport
        Viewport m_Viewport;
    };
}