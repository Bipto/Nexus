#include "ImGuiRenderer.hpp"

#include "Core/Application.hpp"
#include "Platform/DX11/GraphicsDeviceDX11.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_sdl2.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_dx11.h>

namespace Nexus::Graphics
{
    ImGuiRenderer::ImGuiRenderer(Application *app)
    {
        m_Application = app;
    }

    void ImGuiRenderer::Initialise()
    {
        auto graphicsDevice = m_Application->GetGraphicsDevice();
        switch (graphicsDevice->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            InitialiseOpenGL();
            break;
        case GraphicsAPI::DirectX11:
            InitialiseD3D11();
            break;
        }
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplSDL2_NewFrame();
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        case GraphicsAPI::DirectX11:
#if defined(NX_PLATFORM_DX11)
            ImGui_ImplDX11_NewFrame();
#endif
            break;
        }
        ImGui::NewFrame();
    }

    void ImGuiRenderer::EndFrame()
    {
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        case GraphicsAPI::DirectX11:
#if defined(NX_PLATFORM_DX11)
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
            break;
        }
    }

    void ImGuiRenderer::UpdatePlatformWindows()
    {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            m_Application->GetGraphicsDevice()->SetContext();
        }
    }

    void ImGuiRenderer::InitialiseOpenGL()
    {
        const char *glslVersion;
        // Decide GL+GLSL versions
#ifdef __EMSCRIPTEN__
        // GL ES 2.0 + GLSL 100
        glslVersion = "#version 100";
#elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        glslVersion = "#version 150";
#else
        // GL 3.0 + GLSL 130
        glslVersion = "#version 130";
#endif

        SDL_Window *window = m_Application->GetWindow()->GetSDLWindowHandle();
        SDL_GLContext context = (SDL_GLContext)m_Application->GetGraphicsDevice()->GetContext();
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init(glslVersion);
    }

    void ImGuiRenderer::InitialiseD3D11()
    {
#if defined(NX_PLATFORM_DX11)
        ImGui_ImplSDL2_InitForD3D(m_Application->GetWindow()->GetSDLWindowHandle());
        Nexus::Ref<Nexus::Graphics::GraphicsDeviceDX11> device = std::dynamic_pointer_cast<GraphicsDeviceDX11>(m_Application->GetGraphicsDevice());
        ID3D11Device *id3d11Device = (ID3D11Device *)device->GetDevice();
        ID3D11DeviceContext *id3d11DeviceContext = (ID3D11DeviceContext *)device->GetDeviceContext();
        ImGui_ImplDX11_Init(id3d11Device, id3d11DeviceContext);
#endif
    }
}