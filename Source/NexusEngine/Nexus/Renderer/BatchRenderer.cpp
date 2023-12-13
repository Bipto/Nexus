#include "BatchRenderer.hpp"

namespace Nexus::Graphics
{
    BatchRenderer::BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderPass *renderPass)
    {
        m_Device = device;
        m_RenderPass = renderPass;

        m_CommandList = m_Device->CreateCommandList();
        m_Shader = m_Device->CreateShaderFromSpirvFile("resources/shaders/batch.glsl", Nexus::Graphics::VertexPositionTexCoordColorTexIndex::GetLayout());
        CreatePipeline();

        const uint32_t MAX_VERTEX_COUNT = 1024;
        m_Vertices.resize(MAX_VERTEX_COUNT);
        m_Indices.resize(MAX_VERTEX_COUNT * 3);
        m_Textures.resize(MAX_TEXTURES);

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = m_Vertices.size() * sizeof(VertexPositionTexCoordColorTexIndex);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_VertexBuffer = m_Device->CreateVertexBuffer(vertexBufferDesc, nullptr, VertexPositionTexCoordColorTexIndex::GetLayout());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = m_Indices.size() * sizeof(uint32_t);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_IndexBuffer = m_Device->CreateIndexBuffer(indexBufferDesc, nullptr);

        uint32_t textureData = 0xFFFFFFFF;

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.Data = &textureData;
        textureSpec.Width = 1;
        textureSpec.Height = 1;
        textureSpec.Format = TextureFormat::RGBA8;
        textureSpec.NumberOfChannels = 4;
        m_BlankTexture = m_Device->CreateTexture(textureSpec);

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(glm::mat4);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_Device->CreateUniformBuffer(uniformBufferDesc, nullptr);
    }

    void BatchRenderer::Resize()
    {
        CreatePipeline();
    }

    void BatchRenderer::Begin(const Nexus::Graphics::RenderPassBeginInfo &beginInfo, const glm::mat4 &mvp)
    {
        if (m_IsStarted)
        {
            throw std::runtime_error("Batching has already started");
        }

        m_BeginInfo = beginInfo;
        m_IsStarted = true;

        m_Vertices.clear();
        m_Indices.clear();

        m_VertexCount = 0;
        m_IndexCount = 0;
        m_ShapeCount = 0;

        void *buffer = m_UniformBuffer->Map();
        memcpy(buffer, &mvp, sizeof(mvp));
        m_UniformBuffer->Unmap();

        m_Textures.clear();
        m_Textures.push_back(m_BlankTexture);
    }

    void BatchRenderer::DrawRectangle(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color)
    {
        EnsureStarted();

        const uint32_t shapeVertexCount = 4;
        const uint32_t shapeIndexCount = 6;

        EnsureSpace(shapeVertexCount, shapeIndexCount);

        glm::vec3 a(min.x, max.y, 0.0f);
        glm::vec3 b(max.x, max.y, 0.0f);
        glm::vec3 c(max.x, min.y, 0.0f);
        glm::vec3 d(min.x, min.y, 0.0f);

        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(1 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(3 + m_VertexCount);

        VertexPositionTexCoordColorTexIndex v0;
        v0.Position = a;
        v0.TexCoords = {0.0f, 0.0f};
        v0.Color = color;
        v0.TexIndex = 0;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColorTexIndex v1;
        v1.Position = b;
        v1.TexCoords = {1.0f, 0.0f};
        v1.Color = color;
        v1.TexIndex = 0;
        m_Vertices.push_back(v1);

        VertexPositionTexCoordColorTexIndex v2;
        v2.Position = c;
        v2.TexCoords = {1.0f, 1.0f};
        v2.Color = color;
        v2.TexIndex = 0;
        m_Vertices.push_back(v2);

        VertexPositionTexCoordColorTexIndex v3;
        v3.Position = d;
        v3.TexCoords = {0.0f, 1.0f};
        v3.Color = color;
        v3.TexIndex = 0;
        m_Vertices.push_back(v3);

        m_IndexCount += shapeIndexCount;
        m_VertexCount += shapeVertexCount;
    }

    void BatchRenderer::DrawRectangle(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Texture *texture)
    {
        EnsureStarted();

        const uint32_t shapeVertexCount = 4;
        const uint32_t shapeIndexCount = 6;

        EnsureSpace(shapeVertexCount, shapeIndexCount);

        glm::vec3 a(min.x, max.y, 0.0f);
        glm::vec3 b(max.x, max.y, 0.0f);
        glm::vec3 c(max.x, min.y, 0.0f);
        glm::vec3 d(min.x, min.y, 0.0f);

        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(1 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(3 + m_VertexCount);

        float texIndex = (float)m_Textures.size();

        VertexPositionTexCoordColorTexIndex v0;
        v0.Position = a;
        v0.TexCoords = {0.0f, 0.0f};
        v0.Color = color;
        v0.TexIndex = texIndex;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColorTexIndex v1;
        v1.Position = b;
        v1.TexCoords = {1.0f, 0.0f};
        v1.Color = color;
        v1.TexIndex = texIndex;
        m_Vertices.push_back(v1);

        VertexPositionTexCoordColorTexIndex v2;
        v2.Position = c;
        v2.TexCoords = {1.0f, 1.0f};
        v2.Color = color;
        v2.TexIndex = texIndex;
        m_Vertices.push_back(v2);

        VertexPositionTexCoordColorTexIndex v3;
        v3.Position = d;
        v3.TexCoords = {0.0f, 1.0f};
        v3.Color = color;
        v3.TexIndex = texIndex;
        m_Vertices.push_back(v3);

        m_IndexCount += shapeIndexCount;
        m_VertexCount += shapeVertexCount;
        m_Textures.push_back(texture);
    }

    void BatchRenderer::DrawCharacter(char character, const glm::vec2 &position, float scale, const glm::vec4 &color, Font *font)
    {
        EnsureStarted();

        const uint32_t shapeVertexCount = 4;
        const uint32_t shapeIndexCount = 6;

        EnsureSpace(shapeVertexCount, shapeIndexCount);

        const auto &characterInfo = font->GetCharacter(character);
        glm::vec2 min = position;
        glm::vec2 max = {position.x + (characterInfo.Size.x * scale), position.y + (characterInfo.Size.y * scale)};

        glm::vec3 a(min.x, max.y, 0.0f);
        glm::vec3 b(max.x, max.y, 0.0f);
        glm::vec3 c(max.x, min.y, 0.0f);
        glm::vec3 d(min.x, min.y, 0.0f);

        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(1 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(3 + m_VertexCount);

        float texIndex = (float)m_Textures.size();

        VertexPositionTexCoordColorTexIndex v0;
        v0.Position = a;
        v0.TexCoords = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMin.y};
        v0.Color = color;
        v0.TexIndex = texIndex;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColorTexIndex v1;
        v1.Position = b;
        v1.TexCoords = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMin.y};
        v1.Color = color;
        v1.TexIndex = texIndex;
        m_Vertices.push_back(v1);

        VertexPositionTexCoordColorTexIndex v2;
        v2.Position = c;
        v2.TexCoords = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMax.y};
        v2.Color = color;
        v2.TexIndex = texIndex;
        m_Vertices.push_back(v2);

        VertexPositionTexCoordColorTexIndex v3;
        v3.Position = d;
        v3.TexCoords = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMax.y};
        v3.Color = color;
        v3.TexIndex = texIndex;
        m_Vertices.push_back(v3);

        m_IndexCount += shapeIndexCount;
        m_VertexCount += shapeVertexCount;
        m_Textures.push_back(font->GetTexture());
    }

    void BatchRenderer::DrawString(const std::string &text, const glm::vec2 &position, float scale, const glm::vec4 &color, Font *font)
    {
        float x = position.x;
        float y = position.y;

        uint32_t largestCharacterSize = 0;
        for (auto character : text)
        {
            const auto &characterInfo = font->GetCharacter(character);

            auto characterHeight = characterInfo.Size.y * scale;
            if (characterHeight > largestCharacterSize)
            {
                largestCharacterSize = characterHeight;
            }
        }

        for (auto character : text)
        {
            if (character == ' ')
            {
                const auto &characterInfo = font->GetCharacter('i');
                x += characterInfo.Size.x * scale;
            }
            else
            {
                const auto &characterInfo = font->GetCharacter(character);

                float xPos = x + characterInfo.Bearing.x * scale;
                // float yPos = y - (characterInfo.Size.y - characterInfo.Bearing.y) * scale;
                float yPos = (characterInfo.Bearing.y) * scale;
                yPos -= largestCharacterSize;

                DrawCharacter(character, {xPos, y - yPos}, scale, color, font);

                x += (characterInfo.Advance.X >> 6) * scale;
            }
        }
    }

    void BatchRenderer::End()
    {
        EnsureStarted();
        Flush();

        m_IsStarted = false;
    }

    void BatchRenderer::CreatePipeline()
    {
        if (m_Pipeline)
        {
            delete m_Pipeline;
            m_Pipeline = nullptr;
        }

        if (m_ResourceSet)
        {
            delete m_ResourceSet;
            m_ResourceSet = nullptr;
        }

        auto swapchain = m_Device->GetPrimaryWindow();

        Nexus::Graphics::PipelineDescription description;
        description.Shader = m_Shader;
        description.RenderPass = m_RenderPass;
        description.Viewport.X = 0;
        description.Viewport.Y = 0;
        description.Viewport.Width = swapchain->GetWindowSize().X;
        description.Viewport.Height = swapchain->GetWindowSize().Y;
        description.RasterizerStateDescription.ScissorRectangle = {0, 0, swapchain->GetWindowSize().X, swapchain->GetWindowSize().Y};
        description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;

        /* description.BlendStateDescription.EnableBlending = true;
        description.BlendStateDescription.BlendEquation = BlendEquation::Add;
        description.BlendStateDescription.SourceColourBlend = BlendFunction::SourceAlpha;
        description.BlendStateDescription.DestinationColourBlend = BlendFunction::OneMinusSourceAlpha;
        description.BlendStateDescription.SourceAlphaBlend = BlendFunction::One;
        description.BlendStateDescription.DestinationAlphaBlend = BlendFunction::Zero; */

        Nexus::Graphics::ResourceSetSpecification resourceSpec;
        resourceSpec.TextureBindings =
            {
                {0, "texture0"},
                {1, "texture1"},
                {2, "texture2"},
                {3, "texture3"},
                {4, "texture4"},
                {5, "texture5"},
                {6, "texture6"},
                {7, "texture7"},
                {8, "texture8"},
                {9, "texture9"},
                {10, "texture10"},
                {11, "texture11"},
                {12, "texture12"},
                {13, "texture13"},
                {14, "texture14"},
                {15, "texture15"}};

        Nexus::Graphics::UniformResourceBinding uniformResourceBinding;
        uniformResourceBinding.Binding = 0;
        uniformResourceBinding.Name = "MVP";
        uniformResourceBinding.Buffer = m_UniformBuffer;
        resourceSpec.UniformResourceBindings = {uniformResourceBinding};

        description.ResourceSetSpecification = resourceSpec;
        m_Pipeline = m_Device->CreatePipeline(description);
        m_ResourceSet = m_Device->CreateResourceSet(m_Pipeline);
    }

    void BatchRenderer::Flush()
    {
        EnsureStarted();

        void *buffer = m_VertexBuffer->Map();
        memcpy(buffer, m_Vertices.data(), m_Vertices.size() * sizeof(VertexPositionTexCoordColorTexIndex));
        m_VertexBuffer->Unmap();

        buffer = m_IndexBuffer->Map();
        memcpy(buffer, m_Indices.data(), m_Indices.size() * sizeof(uint32_t));
        m_IndexBuffer->Unmap();

        m_ResourceSet->WriteUniformBuffer(m_UniformBuffer, 0);

        for (uint32_t i = 0; i < m_Textures.size(); i++)
        {
            m_ResourceSet->WriteTexture(m_Textures[i], i);
        }

        m_CommandList->Begin();
        m_CommandList->BeginRenderPass(m_RenderPass, m_BeginInfo);
        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->SetResourceSet(m_ResourceSet);

        m_CommandList->SetVertexBuffer(m_VertexBuffer);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);
        m_CommandList->DrawIndexed(m_IndexCount, 0);

        m_CommandList->EndRenderPass();
        m_CommandList->End();
        m_Device->SubmitCommandList(m_CommandList);

        m_Textures.clear();
        m_Textures.push_back(m_BlankTexture);
    }

    void BatchRenderer::EnsureStarted()
    {
        if (!m_IsStarted)
        {
            throw std::runtime_error("Batching was never started");
        }
    }

    void BatchRenderer::EnsureSpace(uint32_t shapeVertexCount, uint32_t shapeIndexCount)
    {
        uint32_t maxVertexCount = m_Vertices.capacity();
        uint32_t maxIndexCount = m_Indices.capacity();

        if (shapeVertexCount > maxVertexCount || shapeIndexCount > maxIndexCount)
        {
            throw std::runtime_error("Max vertex or index count reached for one draw call");
        }

        if (m_VertexCount + shapeVertexCount > maxVertexCount || m_IndexCount + shapeIndexCount > maxIndexCount || m_Textures.size() > MAX_TEXTURES)
        {
            Flush();
        }
    }
}