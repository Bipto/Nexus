#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"

#include "Nexus/Graphics/Font.hpp"
#include "Nexus/Vertex.hpp"
#include "Nexus/Graphics/Rectangle.hpp"
#include "Nexus/Graphics/Circle.hpp"

namespace Nexus::UI
{
    class UIRenderer
    {
    public:
        UIRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderTarget target);
        void Begin() const;
        void SetViewport(const Nexus::Graphics::Viewport &vp) const;
        void SetScissor(const Nexus::Graphics::Scissor &scissor) const;
        void Clear(const Nexus::Graphics::ClearColorValue &clearColor) const;
        void DrawRectangle(const Nexus::Graphics::Rectangle<float> &rect, const glm::vec4 &color);
        void DrawRectangle(const Nexus::Graphics::Rectangle<float> &rect, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec4 &color, Nexus::Ref<Nexus::Graphics::Texture> texture);
        void DrawCharacter(char character, const Nexus::Graphics::Rectangle<float> &rect, const glm::vec4 &color, Nexus::Graphics::Font *font);
        void End() const;

        Nexus::Ref<Nexus::Graphics::CommandList> GetCommandList() const;

    private:
        void CreatePipelines();
        void UpdateBuffers();

    private:
        Nexus::Graphics::GraphicsDevice *m_Device = nullptr;
        Nexus::Graphics::RenderTarget m_RenderTarget;
        Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList = nullptr;

        Nexus::Ref<Nexus::Graphics::Pipeline> m_TexturePipeline = nullptr;
        Nexus::Ref<Nexus::Graphics::ResourceSet> m_TextureResourceSet = nullptr;

        Nexus::Ref<Nexus::Graphics::Pipeline> m_SDFPipeline = nullptr;
        Nexus::Ref<Nexus::Graphics::ResourceSet> m_SDFResourceSet = nullptr;

        std::vector<Nexus::Graphics::VertexPositionTexCoordColor> m_Vertices;
        std::vector<uint32_t> m_Indices;

        Nexus::Ref<Nexus::Graphics::VertexBuffer> m_VertexBuffer = nullptr;
        Nexus::Ref<Nexus::Graphics::IndexBuffer> m_IndexBuffer = nullptr;
        Nexus::Ref<Nexus::Graphics::UniformBuffer> m_UniformBuffer = nullptr;

        Nexus::Ref<Nexus::Graphics::Texture> m_BlankTexture = nullptr;
        Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler = nullptr;
    };
}