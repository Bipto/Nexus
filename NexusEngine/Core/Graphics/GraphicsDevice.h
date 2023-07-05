#pragma once

#include "Core/Window.h"
#include "Core/Memory.h"
#include "SDL.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "CommandList.h"
#include "Core/Graphics/ShaderGenerator.h"
#include "Viewport.h"

namespace Nexus::Graphics
{
    /// @brief An enum class that represents an available graphics API backend
    enum class GraphicsAPI
    {
        /// @brief No graphics API context will be created
        None,

        /// @brief Graphics will be created using OpenGL
        OpenGL,

        /// @brief Graphics will be created using DirectX11
        DirectX11
    };

    /// @brief An enum class representing whether VSync is enabled
    enum class VSyncState
    {
        /// @brief The graphics card will render as many frames as possible or will be limited by the frame rate cap specified
        Disabled = 0,

        /// @brief The refresh rate will be synchronised to the monitor
        Enabled = 1
    };

    /// @brief A class representing properties needed to create a new graphics device
    struct GraphicsDeviceCreateInfo
    {
        /// @brief A raw pointer to the window to use for rendering graphics
        Window *GraphicsWindow;

        /// @brief The chosen graphics API to use to create the GraphicsDevice with
        GraphicsAPI API;

        /// @brief The VSync settings to use when creating the GraphicsDevice
        VSyncState VSyncStateSettings;

        /// @brief An initial viewport to use with the GraphicsDevice
        Viewport GraphicsViewport;
    };

    /// @brief A class representing an abstraction over a graphics API
    class GraphicsDevice
    {
    public:
        /// @brief A constructor taking in a const reference to a GraphicsDeviceCreateInfo
        /// @param createInfo The options to use when creating the GraphicsDevice
        GraphicsDevice(const GraphicsDeviceCreateInfo &createInfo);

        /// @brief A virtual destructor allowing resources to be deleted
        virtual ~GraphicsDevice() {}

        /// @brief Copying a GraphicsDevice is not supported
        /// @param Another GraphicsDevice taken by const reference
        GraphicsDevice(const GraphicsDevice &) = delete;

        /// @brief A pure virtual method that sets makes the current graphics context current
        virtual void SetContext() = 0;

        /// @brief A pure virtual method that binds a framebuffer to a graphics context
        /// @param framebuffer The framebuffer to use for rendering
        virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) = 0;

        /// @brief A pure virtual method that returns the name of the graphics API as a string
        /// @return A const char* containing the API name
        virtual const char *GetAPIName() = 0;

        /// @brief A pure virtual method that will return the name of the device currently being used to render graphics
        /// @return A const char* containing the device name
        virtual const char *GetDeviceName() = 0;

        /// @brief A pure virtual method that will submit a command list for rendering
        /// @param commandList The command list to submit for rendering
        virtual void SubmitCommandList(Ref<CommandList> commandList) = 0;

        /// @brief A pure virtual method that will set the current viewport to render with
        /// @param viewport The viewport to apply
        virtual void SetViewport(const Viewport &viewport) = 0;

        /// @brief A pure virtual method that will return a const reference to the currently bound viewport
        /// @return The current viewport being used for rendering
        virtual const Viewport &GetViewport() = 0;

        /// @brief A pure virtual method that is responsible for initialising ImGui for a given graphics API
        virtual void InitialiseImGui() = 0;

        /// @brief A pure virtual method that is responsible for begininng an ImGui frame render
        virtual void BeginImGuiRender() = 0;

        /// @brief A pure virtual method that is responsible for ending an ImGui frame render
        virtual void EndImGuiRender() = 0;

        /// @brief A pure virtual method that returns a context for a graphics API
        /// @return A void pointer to the graphics context
        virtual void *GetContext() = 0;

        /// @brief A method that returns an enum value representing the currently running graphics API backend
        /// @return A GraphicsAPI enum containing the current backend
        GraphicsAPI GetGraphicsAPI() { return this->m_API; }

        /// @brief A pure virtual method that creates a new shader from a vertex and fragment shader and a vertex buffer layout
        /// @param vertexShaderSource A vertex shader written in the native shading language of the backend
        /// @param fragmentShaderSource A fragment shader written in the native shading language of the backend
        /// @param layout A parameter specifying how data is laid out in the vertex buffer
        /// @return A reference counted pointer to a shader
        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) = 0;

        /// @brief A pure virtual method that creates a vertex buffer with a given input vector of vertices
        /// @param vertices A vector containing the vertices to use in the vertex buffer
        /// @return A reference counted pointer to a vertex buffer
        virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<Vertex> vertices) = 0;

        /// @brief A pure virtual method that creates an index buffer with a given input vector of indices
        /// @param indices A vector containing the indices to use in the index buffer
        /// @return A reference counted pointer to an index buffer
        virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) = 0;

        /// @brief A pure virtual method that creates a uniform buffer with a specified binding
        /// @param binding A set of parameters that defines how the buffer will be accessed from a shader and how large it will be
        /// @return A reference counted pointer to a uniform buffer
        virtual Ref<UniformBuffer> CreateUniformBuffer(const UniformResourceBinding &binding) = 0;

        /// @brief A pure virtual method that creates a pipeline from a given pipeline description
        /// @param description The properties to use when creating the pipeline
        /// @return A reference counted pointer to a pipeline
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) = 0;

        /// @brief A pure virtual method that creates a new command list
        /// @return A reference counted pointer to a command list
        virtual Ref<CommandList> CreateCommandList() = 0;

        /// @brief A pure virtual method that creates a new texture from a given specification
        /// @param spec The properties to use when creating the texture
        /// @return A reference counted pointer to a texture
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) = 0;

        /// @brief A method that loads a new texture from a image stored on disk
        /// @param filepath The filepath to load the image from
        /// @return A reference counted pointer to a texture
        Ref<Texture> CreateTexture(const char *filepath);

        /// @brief A pure virtual method that creates a new framebuffer from a given specification
        /// @param spec The properties to use when creating the framebuffer
        /// @return A reference counted pointer to a framebuffer
        virtual Ref<Graphics::Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) = 0;

        /// @brief A pure virtual method that resizes the swapchain of the device to a given size
        /// @param size The new size of the swapchain
        virtual void Resize(Point<int> size) = 0;

        /// @brief A pure virtual method that swaps the buffers and presents the backbuffer to the screen
        virtual void SwapBuffers() = 0;

        /// @brief A pure virtual method that sets the VSync state of the graphics device
        /// @param vSyncState An enum that enables or disables VSync
        virtual void SetVSyncState(VSyncState vSyncState) = 0;

        /// @brief A pure virtual method that returns the current VSync state of the device
        /// @return An enum representing whether VSync is enabled or disabled
        virtual VSyncState GetVsyncState() = 0;

        /// @brief A pure virtual method that returns a ShaderFormat enum representing the supported shading language of the backend
        /// @return The supported shading language of the backend
        virtual ShaderLanguage GetSupportedShaderFormat() = 0;

        /// @brief A pure virtual method that returns a value that can be used to standardise UV coordinates across backends
        /// @return A float representing the correction
        virtual float GetUVCorrection() = 0;

        /// @brief A method that generates a supported shader type from a GLSL file and a vertex buffer layout
        /// @param filepath The filepath to load the GLSL shader from
        /// @param layout A vertex buffer layout to use to create the shader
        /// @return A reference counted pointer to a shader
        Ref<Shader> CreateShaderFromSpirvFile(const std::string &filepath, const VertexBufferLayout &layout);

    protected:
        /// @brief A pointer to the window to render graphics into
        Nexus::Window *m_Window;

        /// @brief A value representing the graphics API being used by the GraphicsDevice
        GraphicsAPI m_API;

        /// @brief The currently used viewport
        Viewport m_Viewport;
    };
}