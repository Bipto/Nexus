#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"

#include "Nexus/Vertex.hpp"

namespace Nexus::Graphics
{
    struct VertexPositionTexCoordColorTexIndex
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec2 TexCoords = {0, 0};
        glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
        float TexIndex = 0.0f;

        VertexPositionTexCoordColorTexIndex() = default;

        VertexPositionTexCoordColorTexIndex(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec4 &color, float texIndex)
            : Position(position), TexCoords(texCoords), Color(color), TexIndex(texIndex) {}

        static Nexus::Graphics::VertexBufferLayout GetLayout()
        {
            Nexus::Graphics::VertexBufferLayout layout =
                {
                    {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
                    {Nexus::Graphics::ShaderDataType::Float, "TEXCOORD"}};
            return layout;
        }
    };

    class BatchRenderer
    {
    public:
        BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderPass *renderPass);

        void Resize();

        void Begin(const Nexus::Graphics::RenderPassBeginInfo &beginInfo, const glm::mat4 &mvp);
        void DrawRectangle(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color);
        void DrawRectangle(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Texture *texture);
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

        std::vector<Nexus::Graphics::VertexPositionTexCoordColorTexIndex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        std::vector<Nexus::Graphics::Texture *> m_Textures;

        uint32_t m_ShapeCount = 0;
        uint32_t m_VertexCount = 0;
        uint32_t m_IndexCount = 0;
        bool m_IsStarted = false;

        Nexus::Graphics::VertexBuffer *m_VertexBuffer = nullptr;
        Nexus::Graphics::IndexBuffer *m_IndexBuffer = nullptr;
        Nexus::Graphics::UniformBuffer *m_UniformBuffer = nullptr;

        Nexus::Graphics::Texture *m_BlankTexture = nullptr;
        const uint32_t MAX_TEXTURES = 32;
    };
}