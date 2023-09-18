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

        /// @brief A method to begin a new ImGui frame
        void BeginFrame();

        /// @brief A method to end an ImGui frame
        void EndFrame();

        /// @brief A method to update and render ImGui platform windows
        void UpdatePlatformWindows();

        ImTextureID BindTexture(Ref<Texture> texture);

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