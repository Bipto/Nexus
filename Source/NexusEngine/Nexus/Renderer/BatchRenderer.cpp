#include "BatchRenderer.hpp"

namespace Nexus::Graphics
{
    BatchRenderer::BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderPass *renderPass)
    {
        m_Device = device;
        m_RenderPass = renderPass;

        m_CommandList = m_Device->CreateCommandList();
        m_Shader = m_Device->CreateShaderFromSpirvFile("resources/shaders/batch.glsl", Nexus::Graphics::VertexPositionTexCoordColor::GetLayout());
        CreatePipeline();

        const uint32_t MAX_VERTEX_COUNT = 1024;
        m_Vertices.resize(MAX_VERTEX_COUNT);
        m_Indices.resize(MAX_VERTEX_COUNT * 3);

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = m_Vertices.size() * sizeof(VertexPositionTexCoordColor);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_VertexBuffer = m_Device->CreateVertexBuffer(vertexBufferDesc, nullptr, VertexPositionTexCoordColor::GetLayout());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = m_Indices.size() * sizeof(uint32_t);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_IndexBuffer = m_Device->CreateIndexBuffer(indexBufferDesc, nullptr);
    }

    void BatchRenderer::Resize()
    {
        CreatePipeline();
    }

    void BatchRenderer::Begin(const Nexus::Graphics::RenderPassBeginInfo &beginInfo)
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

        VertexPositionTexCoordColor v0;
        v0.Position = a;
        v0.TexCoords = {1.0f, 1.0f};
        v0.Color = color;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColor v1;
        v0.Position = b;
        v0.TexCoords = {1.0f, 1.0f};
        v0.Color = color;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColor v2;
        v0.Position = c;
        v0.TexCoords = {1.0f, 1.0f};
        v0.Color = color;
        m_Vertices.push_back(v0);

        VertexPositionTexCoordColor v3;
        v0.Position = d;
        v0.TexCoords = {1.0f, 1.0f};
        v0.Color = color;
        m_Vertices.push_back(v0);

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
        description.RenderPass = m_RenderPass;
        description.Viewport.X = 0;
        description.Viewport.Y = 0;
        description.Viewport.Width = swapchain->GetWindowSize().X;
        description.Viewport.Height = swapchain->GetWindowSize().Y;
        description.RasterizerStateDescription.ScissorRectangle = {0, 0, swapchain->GetWindowSize().X, swapchain->GetWindowSize().Y};
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

        m_Pipeline = m_Device->CreatePipeline(description);
    }

    void BatchRenderer::Flush()
    {
        EnsureStarted();

        void *buffer = m_VertexBuffer->Map();
        memcpy(buffer, m_Vertices.data(), m_Vertices.size() * sizeof(VertexPositionTexCoordColor));
        m_VertexBuffer->Unmap();

        buffer = m_IndexBuffer->Map();
        memcpy(buffer, m_Indices.data(), m_Indices.size() * sizeof(uint32_t));
        m_IndexBuffer->Unmap();

        m_CommandList->Begin();
        m_CommandList->BeginRenderPass(m_RenderPass, m_BeginInfo);
        m_CommandList->SetPipeline(m_Pipeline);

        m_CommandList->SetVertexBuffer(m_VertexBuffer);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);
        m_CommandList->DrawIndexed(m_IndexCount, 0);

        m_CommandList->EndRenderPass();
        m_CommandList->End();
        m_Device->SubmitCommandList(m_CommandList);
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

        if (m_VertexCount + shapeVertexCount > maxVertexCount || m_IndexCount + shapeIndexCount > maxIndexCount)
        {
            Flush();
        }
    }
}