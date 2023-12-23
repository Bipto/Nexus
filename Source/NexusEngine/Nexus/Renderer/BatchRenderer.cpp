#include "BatchRenderer.hpp"

#include "glm/gtx/compatibility.hpp"

namespace Nexus::Graphics
{
    BatchRenderer::BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderTarget target)
    {
        m_Device = device;
        m_RenderTarget = target;

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
        m_Width = m_Device->GetPrimaryWindow()->GetWindowSize().X;
        m_Height = m_Device->GetPrimaryWindow()->GetWindowSize().Y;
    }

    void BatchRenderer::Begin(const glm::mat4 &mvp)
    {
        if (m_IsStarted)
        {
            throw std::runtime_error("Batching has already started");
        }

        m_IsStarted = true;

        m_Vertices.clear();
        m_Indices.clear();

        m_VertexCount = 0;
        m_IndexCount = 0;
        m_ShapeCount = 0;

        m_Viewport.X = 0;
        m_Viewport.Y = 0;
        m_Viewport.Width = m_RenderTarget.GetSize().X;
        m_Viewport.Height = m_RenderTarget.GetSize().Y;

        m_ScissorRectangle.X = 0;
        m_ScissorRectangle.Y = 0;
        m_ScissorRectangle.Width = m_RenderTarget.GetSize().X;
        m_ScissorRectangle.Height = m_RenderTarget.GetSize().Y;

        glm::mat4 projection = glm::ortho<float>(m_Viewport.X, m_Viewport.Width, m_Viewport.Height, m_Viewport.Y, -1.0f, 1.0f);

        void *buffer = m_UniformBuffer->Map();
        memcpy(buffer, &projection, sizeof(projection));
        m_UniformBuffer->Unmap();

        m_Textures.clear();
        m_Textures.push_back(m_BlankTexture);
    }

    void BatchRenderer::Begin(Viewport viewport, Scissor scissor)
    {
        if (m_IsStarted)
        {
            throw std::runtime_error("Batching has already started");
        }

        m_IsStarted = true;

        m_Vertices.clear();
        m_Indices.clear();

        m_VertexCount = 0;
        m_IndexCount = 0;
        m_ShapeCount = 0;

        m_Viewport = viewport;
        m_ScissorRectangle = scissor;

        glm::mat4 projection = glm::ortho<float>(m_Viewport.X, m_Viewport.Width, m_Viewport.Height, m_Viewport.Y, -1.0f, 1.0f);

        void *buffer = m_UniformBuffer->Map();
        memcpy(buffer, &projection, sizeof(projection));
        m_UniformBuffer->Unmap();

        m_Textures.clear();
        m_Textures.push_back(m_BlankTexture);
    }

    void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color)
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

    void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Texture *texture)
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

        float texIndex = 0.0f;

        uint32_t index;
        if (FindTextureInBatch(texture, &index))
        {
            texIndex = (float)index;
        }
        else
        {
            texIndex = (float)m_Textures.size();
            m_Textures.push_back(texture);
        }

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
    }

    void BatchRenderer::DrawQuadFill(const Rectangle &rectangle, const glm::vec4 &color)
    {
        glm::vec2 min = {(float)rectangle.GetLeft(), (float)rectangle.GetTop()};
        glm::vec2 max = {(float)rectangle.GetRight(), (float)rectangle.GetBottom()};
        DrawQuadFill(min, max, color);
    }

    void BatchRenderer::DrawQuad(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, float thickness)
    {
        DrawLine({min.x, max.y}, {max.x, max.y}, color, thickness);
        DrawLine({max.x, max.y}, {max.x, min.y}, color, thickness);
        DrawLine({max.x, min.y}, {min.x, min.y}, color, thickness);
        DrawLine({min.x, min.y}, {min.x, max.y}, color, thickness);
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

        float texIndex = 0.0f;

        uint32_t index;
        auto texture = font->GetTexture();
        if (FindTextureInBatch(texture, &index))
        {
            texIndex = (float)index;
        }
        else
        {
            texIndex = (float)m_Textures.size();
            m_Textures.push_back(texture);
        }

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
    }

    void BatchRenderer::DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font)
    {
        float scale = 1.0f / font->GetSize() * size;

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
                x += font->GetSpaceWidth() * scale;
            }
            else if (character == '\n')
            {
                x = position.x;
                y += font->GetLargestCharacterSize().y * scale;
            }
            else if (character == '\t')
            {
                x += font->GetSpaceWidth() * scale * 4;
            }
            else
            {
                const auto &characterInfo = font->GetCharacter(character);

                float xPos = x + characterInfo.Bearing.x * scale;
                float yPos = (characterInfo.Bearing.y) * scale;
                yPos -= largestCharacterSize;

                DrawCharacter(character, {xPos, y - yPos}, scale, color, font);

                x += (characterInfo.Advance.x / 64) * scale;
            }
        }
    }

    void BatchRenderer::DrawLine(const glm::vec2 &a, const glm::vec2 &b, const glm::vec4 &color, float thickness)
    {
        EnsureStarted();

        const uint32_t shapeVertexCount = 4;
        const uint32_t shapeIndexCount = 6;

        EnsureSpace(shapeVertexCount, shapeIndexCount);

        thickness = glm::clamp(thickness, 1.0f, 10.0f);

        float halfThickness = thickness / 2.0f;

        glm::vec2 e1 = b - a;
        e1 = glm::normalize(e1);
        e1 *= halfThickness;

        glm::vec2 e2 = -e1;

        glm::vec2 n1 = {-e1.y, e1.x};
        glm::vec2 n2 = -n1;

        glm::vec2 q1 = a + n1 + e2;
        glm::vec2 q2 = b + n1 + e1;
        glm::vec2 q3 = b + n2 + e1;
        glm::vec2 q4 = a + n2 + e2;

        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(1 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(0 + m_VertexCount);
        m_Indices.push_back(2 + m_VertexCount);
        m_Indices.push_back(3 + m_VertexCount);

        VertexPositionTexCoordColorTexIndex v0;
        v0.Position = glm::vec3(q1, 0.0f);
        v0.TexCoords = {0.0f, 0.0f};
        v0.Color = color;
        v0.TexIndex = 0;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColorTexIndex v1;
        v1.Position = glm::vec3(q2, 0.0f);
        v1.TexCoords = {1.0f, 0.0f};
        v1.Color = color;
        v1.TexIndex = 0;
        m_Vertices.push_back(v1);

        VertexPositionTexCoordColorTexIndex v2;
        v2.Position = glm::vec3(q3, 0.0f);
        v2.TexCoords = {1.0f, 1.0f};
        v2.Color = color;
        v2.TexIndex = 0;
        m_Vertices.push_back(v2);

        VertexPositionTexCoordColorTexIndex v3;
        v3.Position = glm::vec3(q4, 0.0f);
        v3.TexCoords = {0.0f, 1.0f};
        v3.Color = color;
        v3.TexIndex = 0;
        m_Vertices.push_back(v3);

        m_IndexCount += shapeIndexCount;
        m_VertexCount += shapeVertexCount;
    }

    void BatchRenderer::DrawCircle(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float thickness)
    {
        const uint32_t minPoints = 3;
        const uint32_t maxPoints = 256;

        numberOfPoints = glm::clamp<float>(numberOfPoints, minPoints, maxPoints);

        float deltaAngle = glm::two_pi<float>() / (float)numberOfPoints;
        float angle = 0.0f;

        for (int i = 0; i < numberOfPoints; i++)
        {
            float ax = glm::sin(angle) * radius + (position.x / 2);
            float ay = glm::cos(angle) * radius + (position.y / 2);

            angle += deltaAngle;

            float bx = glm::sin(angle) * radius + (position.x / 2);
            float by = glm::cos(angle) * radius + (position.y / 2);

            DrawLine({ax, ay}, {bx, by}, color, thickness);
        }
    }

    void BatchRenderer::DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints)
    {
        const uint32_t minPoints = 3;
        const uint32_t maxPoints = 256;

        int shapeVertexCount = glm::clamp(numberOfPoints, minPoints, maxPoints);
        int shapeTriangleCount = shapeVertexCount - 2;
        int shapeIndexCount = shapeTriangleCount * 3;

        EnsureSpace(shapeVertexCount, shapeIndexCount);

        // create indices
        int index = 1;

        for (int i = 0; i < shapeTriangleCount; i++)
        {
            m_Indices.push_back(0 + m_VertexCount);
            m_Indices.push_back(index + m_VertexCount);
            m_Indices.push_back(index + 1 + m_VertexCount);

            index++;
        }

        // create vertices
        float rotation = glm::two_pi<float>() / (float)shapeVertexCount;

        float sin = glm::sin(rotation);
        float cos = glm::cos(rotation);

        float ax = radius;
        float ay = 0.0f;

        for (int i = 0; i < shapeVertexCount; i++)
        {
            float x1 = ax;
            float y1 = ay;

            VertexPositionTexCoordColorTexIndex vertex;
            vertex.Position = glm::vec3{x1 + position.x, y1 + position.y, 0.0f};
            vertex.TexCoords = {0.0f, 0.0f};
            vertex.Color = color;
            vertex.TexIndex = 0;
            m_Vertices.push_back(vertex);

            ax = cos * x1 - sin * y1;
            ay = sin * x1 + cos * y1;
        }

        m_IndexCount += shapeIndexCount;
        m_VertexCount += shapeVertexCount;
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
        description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;

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
        description.Target = {m_RenderTarget};
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

        for (uint32_t i = m_Textures.size(); i < MAX_TEXTURES; i++)
        {
            m_ResourceSet->WriteTexture(m_BlankTexture, i);
        }

        m_CommandList->Begin();
        m_CommandList->SetPipeline(m_Pipeline);

        /* m_CommandList->SetViewport(m_Viewport);
        m_CommandList->SetScissor(m_ScissorRectangle); */

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = m_Device->GetPrimaryWindow()->GetWindowSize().X;
        vp.Height = m_Device->GetPrimaryWindow()->GetWindowSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        // m_CommandList->SetViewport(vp);

        Nexus::Graphics::Scissor scissor;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = m_Device->GetPrimaryWindow()->GetWindowSize().X;
        scissor.Height = m_Device->GetPrimaryWindow()->GetWindowSize().Y;
        // m_CommandList->SetScissor(scissor);

        m_CommandList->SetViewport(m_Viewport);
        m_CommandList->SetScissor(m_ScissorRectangle);

        m_CommandList->SetResourceSet(m_ResourceSet);

        m_CommandList->SetVertexBuffer(m_VertexBuffer);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);
        m_CommandList->DrawIndexed(m_IndexCount, 0);

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

    bool BatchRenderer::FindTextureInBatch(Texture *texture, uint32_t *index)
    {
        for (int i = 0; i < m_Textures.size(); i++)
        {
            if (m_Textures[i] == texture)
            {
                if (index)
                {
                    *index = (uint32_t)i;
                }

                return true;
            }
        }

        return false;
    }
}