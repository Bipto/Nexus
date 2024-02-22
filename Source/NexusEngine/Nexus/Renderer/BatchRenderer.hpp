#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"

#include "Nexus/Graphics/Font.hpp"
#include "Nexus/Vertex.hpp"
#include "Nexus/Graphics/Rectangle.hpp"
#include "Nexus/Graphics/Circle.hpp"

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
        BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderTarget target);

        void Resize();

        void Begin(const glm::mat4 &mvp);
        void Begin(Viewport viewport, Scissor scissor);
        void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color);
        void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Texture *texture);
        void DrawQuadFill(const Rectangle &rectangle, const glm::vec4 &color);
        void DrawQuad(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, float thickness);
        void DrawCharacter(char character, const glm::vec2 &position, float scale, const glm::vec4 &color, Font *font);
        void DrawCharacter(char character, const glm::vec2 &position, float scale, const glm::vec4 &color, Font *font, const Rectangle &clippingRectangle);
        void DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font);
        void DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font, const Rectangle &clippingRectangle);
        void DrawLine(const glm::vec2 &a, const glm::vec2 &b, const glm::vec4 &color, float thickness);
        void DrawCircle(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float thickness);
        void DrawCircle(const Circle &circle, const glm::vec4 &color, uint32_t numberOfPoints, float thickness);
        void DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints);
        void DrawCircleFill(const Circle &circle, const glm::vec4 &color, uint32_t numberOfPoints);
        void DrawCross(const Rectangle &rectangle, float thickness, const glm::vec4 &color);
        void DrawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c, const glm::vec4 &color);
        void End();

    private:
        void CreatePipeline();
        void Flush();
        void EnsureStarted();
        void EnsureSpace(uint32_t shapeVertexCount, uint32_t shapeIndexCount);
        bool FindTextureInBatch(Texture *texture, uint32_t *index);

    private:
        Nexus::Graphics::GraphicsDevice *m_Device = nullptr;
        Nexus::Graphics::Shader *m_Shader = nullptr;
        Nexus::Graphics::CommandList *m_CommandList = nullptr;

        Nexus::Graphics::Pipeline *m_Pipeline = nullptr;
        Nexus::Graphics::ResourceSet *m_ResourceSet = nullptr;
        Nexus::Graphics::Sampler *m_Sampler = nullptr;

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
        const uint32_t MAX_TEXTURES = 16;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        Nexus::Graphics::RenderTarget m_RenderTarget;

        Nexus::Graphics::Viewport m_Viewport;
        Nexus::Graphics::Scissor m_ScissorRectangle;
    };
}