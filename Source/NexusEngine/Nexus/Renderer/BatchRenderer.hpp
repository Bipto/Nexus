#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"

#include "Nexus/Vertex.hpp"

namespace Nexus::Graphics
{
    class BatchRenderer
    {
    public:
        BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderPass *renderPass);

        void Resize();

        void Begin(const Nexus::Graphics::RenderPassBeginInfo &beginInfo);
        void DrawRectangle(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color);
        void End();

    private:
        void CreatePipeline();
        void Flush();
        void EnsureStarted();
        void EnsureSpace(uint32_t shapeVertexCount, uint32_t shapeIndexCount);

    private:
        Nexus::Graphics::GraphicsDevice *m_Device = nullptr;
        Nexus::Graphics::RenderPass *m_RenderPass = nullptr;
        Nexus::Graphics::Shader *m_Shader = nullptr;
        Nexus::Graphics::CommandList *m_CommandList = nullptr;
        Nexus::Graphics::RenderPassBeginInfo m_BeginInfo;

        Nexus::Graphics::Pipeline *m_Pipeline = nullptr;
        Nexus::Graphics::ResourceSet *m_ResourceSet = nullptr;

        std::vector<Nexus::Graphics::VertexPositionTexCoordColor> m_Vertices;
        std::vector<uint32_t> m_Indices;

        uint32_t m_ShapeCount = 0;
        uint32_t m_VertexCount = 0;
        uint32_t m_IndexCount = 0;
        bool m_IsStarted = false;

        Nexus::Graphics::VertexBuffer *m_VertexBuffer = nullptr;
        Nexus::Graphics::IndexBuffer *m_IndexBuffer = nullptr;
    };
}