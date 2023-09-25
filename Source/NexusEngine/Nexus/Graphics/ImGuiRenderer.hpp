#pragma once

#include "GraphicsDevice.hpp"
#include "Nexus/Memory.hpp"

namespace Nexus
{
    // forward declaration of Application
    class Application;
}

namespace Nexus::Graphics
{
    /// @brief A class to abstract the complexities of setting up ImGui for different platforms and backends
    class ImGuiRenderer
    {
    public:
        /// @brief A constructor taking in an application
        /// @param app The application to use to create the renderer
        ImGuiRenderer(Application *app);

        /// @brief A default constructor cannot be created
        ImGuiRenderer() = delete;

        /// @brief A method to initialise ImGui
        void Initialise();

        /// @brief A method to shutdown ImGui
        void Shutdown();

        /// @brief A method to begin a new ImGui frame
        void BeginFrame();

        /// @brief A method to end an ImGui frame
        void EndFrame();

        /// @brief A method to update and render ImGui platform windows
        void UpdatePlatformWindows();

        /// @brief A method that binds a API specific texture to an ImTextureID
        /// @param texture A pointer to the texture to bind
        /// @return An ImTextureID that binds to the API specific texture
        ImTextureID BindTexture(Texture *texture);

        /// @brief A method that binds an API specific framebuffer target to an ImTextureID
        /// @param framebuffer A pointer to a framebuffer
        /// @param textureIndex The index of the texture to retrieve
        /// @return An ImTextureID that binds to the API specific texture
        ImTextureID BindFramebufferTexture(Framebuffer *framebuffer, uint32_t textureIndex);

    private:
        /// @brief A private method to initialise ImGui for OpenGL
        void InitialiseOpenGL();

        /// @brief A private method to initialise ImGui for DirectX11
        void InitialiseD3D11();

        /// @brief A private method to initialise ImGui for Vulkan
        void InitialiseVulkan();

        /// @brief A private method to contain all functionality for ending a frame with Vulkan
        void EndFrameImplVulkan();

    private:
        /// @brief A pointer to an Application
        Application *m_Application = nullptr;
    };
}