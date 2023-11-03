#include "NexusEngine.hpp"

#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"

std::vector<Nexus::Graphics::VertexPositionTexCoord> vertices =
    {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{0.0f, 0.5f, 0.0f}, {0.5f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}}};

std::vector<uint32_t> indices = {0, 1, 2};

struct TestUniforms
{
    glm::mat4 Transform;
};

class TestApplication : public Nexus::Application
{
public:
    TestApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
    }

    virtual void Load() override
    {
        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/basic.glsl", Nexus::Graphics::VertexPositionTexCoord::GetLayout());

        Nexus::Graphics::RenderPassSpecification renderPassSpec;
        renderPassSpec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        m_RenderPass = m_GraphicsDevice->CreateRenderPass(renderPassSpec, GetPrimaryWindow()->GetSwapchain());

        CreatePipeline(GetWindowSize());

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = vertices.size() * sizeof(Nexus::Graphics::VertexPositionTexCoord);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data(), Nexus::Graphics::VertexPositionTexCoord::GetLayout());

        Nexus::Graphics::BufferDescription indexBufferDesc;
        indexBufferDesc.Size = indices.size() * sizeof(uint32_t);
        indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(indexBufferDesc, indices.data());

        m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");

        m_CommandList = m_GraphicsDevice->CreateCommandList();
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_ResourceSet->WriteTexture(m_Texture, 0);

        m_GraphicsDevice->BeginFrame();

        // Nexus::Graphics::GraphicsDeviceD3D12 *d3d12Device = (Nexus::Graphics::GraphicsDeviceD3D12 *)m_GraphicsDevice;
        // d3d12Device->Draw(m_Pipeline, m_VertexBuffer);

        m_CommandList->Begin();

        Nexus::Graphics::RenderPassBeginInfo beginInfo{};
        beginInfo.ClearColorValue = {
            0.45f,
            0.32f,
            0.55f,
            1.0f};
        m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
        {
            m_CommandList->SetPipeline(m_Pipeline);
            m_CommandList->SetResourceSet(m_ResourceSet);
            m_CommandList->SetVertexBuffer(m_VertexBuffer);
            m_CommandList->SetIndexBuffer(m_IndexBuffer);
            m_CommandList->DrawIndexed(3, 0);
        }
        m_CommandList->EndRenderPass();
        m_CommandList->End();
        m_GraphicsDevice->EndFrame();

        m_GraphicsDevice->SubmitCommandList(m_CommandList);
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        CreatePipeline(size);
    }

    void CreatePipeline(Nexus::Point<int> size)
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

        Nexus::Graphics::PipelineDescription description;
        description.Shader = m_Shader;
        description.RenderPass = m_RenderPass;
        description.Viewport.X = 0;
        description.Viewport.Y = 0;
        description.Viewport.Width = size.X;
        description.Viewport.Height = size.Y;
        description.RasterizerStateDescription.ScissorRectangle = {0, 0, size.X, size.Y};
        description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
        description.RenderPass = m_RenderPass;

        Nexus::Graphics::TextureResourceBinding textureBinding;
        textureBinding.Name = "texSampler";
        textureBinding.Slot = 0;

        Nexus::Graphics::ResourceSetSpecification resourceSetSpec;
        resourceSetSpec.TextureBindings = {textureBinding};
        description.ResourceSetSpecification = resourceSetSpec;

        m_Pipeline = m_GraphicsDevice->CreatePipeline(description);
        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
    }

    virtual void Unload() override
    {
        delete m_Shader;
        delete m_Pipeline;
        delete m_VertexBuffer;
        delete m_CommandList;
        delete m_RenderPass;
        delete m_Texture;
    }

private:
    Nexus::Graphics::CommandList *m_CommandList = nullptr;
    Nexus::Graphics::RenderPass *m_RenderPass = nullptr;

    Nexus::Graphics::Shader *m_Shader = nullptr;
    Nexus::Graphics::Pipeline *m_Pipeline = nullptr;

    Nexus::Graphics::VertexBuffer *m_VertexBuffer = nullptr;
    Nexus::Graphics::IndexBuffer *m_IndexBuffer = nullptr;

    Nexus::Graphics::ResourceSet *m_ResourceSet = nullptr;
    Nexus::Graphics::Texture *m_Texture = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::D3D12;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = false;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new TestApplication(spec);
}