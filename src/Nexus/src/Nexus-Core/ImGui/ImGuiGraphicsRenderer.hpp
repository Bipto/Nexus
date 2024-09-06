#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/ImGui/ImGuiInclude.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::ImGuiUtils
{
struct ImGuiWindowInfo
{
    Nexus::Window *Window = nullptr;

    virtual ~ImGuiWindowInfo()
    {
        Window->Close();
    }
};

class ImGuiGraphicsRenderer
{
  public:
    ImGuiGraphicsRenderer(Nexus::Application *app);
    virtual ~ImGuiGraphicsRenderer();
    void RebuildFontAtlas();

    ImTextureID BindTexture(Nexus::Ref<Nexus::Graphics::Texture2D> texture);
    void UnbindTexture(ImTextureID id);

    void BeforeLayout(Nexus::Time gameTime);
    void AfterLayout();

    Ref<Graphics::Pipeline> GetPipeline()
    {
        return m_Pipeline;
    }

  private:
    void SetupInput();
    void UpdateInput();
    void RenderDrawData(ImDrawData *drawData);
    void UpdateBuffers(ImDrawData *drawData);
    void RenderCommandLists(ImDrawData *drawData);
    void UpdateCursor();
    void UpdateMonitors();

  private:
    Nexus::Application *m_Application = nullptr;
    Nexus::Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList = nullptr;
    Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline = nullptr;
    Nexus::Ref<Nexus::Graphics::Texture2D> m_FontTexture = nullptr;

    std::map<ImTextureID, Nexus::Ref<Nexus::Graphics::ResourceSet>> m_ResourceSets;
    Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler = nullptr;
    uint64_t m_TextureID = 0;
    ImTextureID m_FontTextureID = 0;

    Nexus::Ref<Nexus::Graphics::VertexBuffer> m_VertexBuffer = nullptr;
    uint32_t m_VertexBufferCount = 0;

    Nexus::Ref<Nexus::Graphics::IndexBuffer> m_IndexBuffer = nullptr;
    uint32_t m_IndexBufferCount = 0;

    Nexus::Ref<Nexus::Graphics::UniformBuffer> m_UniformBuffer = nullptr;

    std::vector<int> m_Keys;
};
} // namespace Nexus::ImGuiUtils