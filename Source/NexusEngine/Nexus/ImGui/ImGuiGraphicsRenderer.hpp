#pragma once

#include "Nexus/Application.hpp"
#include "Nexus/Graphics/GraphicsDevice.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <map>

namespace Nexus::ImGuiUtils
{
    class ImGuiGraphicsRenderer
    {
    public:
        ImGuiGraphicsRenderer(Nexus::Application *app);
        virtual ~ImGuiGraphicsRenderer();
        void RebuildFontAtlas();

        ImTextureID BindTexture(Nexus::Graphics::Texture *texture);
        void UnbindTexture(ImTextureID id);

        void BeforeLayout(Nexus::Time gameTime);
        void AfterLayout();

    private:
        void SetupInput();
        void UpdateInput();
        void RenderDrawData(ImDrawData *drawData);
        void UpdateBuffers(ImDrawData *drawData);
        void RenderCommandLists(ImDrawData *drawData);
        void UpdateCursor();

    private:
        Nexus::Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;
        Nexus::Graphics::CommandList *m_CommandList = nullptr;
        Nexus::Graphics::Shader *m_Shader = nullptr;
        Nexus::Graphics::Pipeline *m_Pipeline = nullptr;
        Nexus::Graphics::Texture *m_FontTexture = nullptr;

        std::map<ImTextureID, Nexus::Graphics::ResourceSet *> m_ResourceSets;
        Nexus::Graphics::Sampler *m_Sampler = nullptr;
        uint64_t m_TextureID = 0;
        ImTextureID m_FontTextureID = 0;

        Nexus::Graphics::VertexBuffer *m_VertexBuffer = nullptr;
        uint32_t m_VertexBufferCount = 0;

        Nexus::Graphics::IndexBuffer *m_IndexBuffer = nullptr;
        uint32_t m_IndexBufferCount = 0;

        Nexus::Graphics::UniformBuffer *m_UniformBuffer = nullptr;

        std::vector<int> m_Keys;
    };
}