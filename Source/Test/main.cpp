#include "NexusEngine.hpp"

#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"

std::vector<Nexus::Graphics::VertexPosition> vertices =
    {
        {{-0.5f, 0.5f, 0.0f}},
        {{0.0f, -0.5f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}}};

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
        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/basic.glsl", Nexus::Graphics::VertexPosition::GetLayout());
        CreatePipeline(GetWindowSize());

        Nexus::Graphics::BufferDescription vertexBufferDesc;
        vertexBufferDesc.Size = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
        vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data(), Nexus::Graphics::VertexPosition::GetLayout());
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->BeginFrame();

        Nexus::Graphics::GraphicsDeviceD3D12 *d3d12Device = (Nexus::Graphics::GraphicsDeviceD3D12 *)m_GraphicsDevice;
        d3d12Device->Draw(m_Pipeline, m_VertexBuffer);

        m_GraphicsDevice->EndFrame();
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

        Nexus::Graphics::PipelineDescription description;
        description.Shader = m_Shader;
        description.RenderPass = m_RenderPass;
        description.Viewport.X = 0;
        description.Viewport.Y = 0;
        description.Viewport.Width = size.X;
        description.Viewport.Height = size.Y;
        description.RasterizerStateDescription.ScissorRectangle = {0, 0, size.X, size.Y};
        description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;

        Nexus::Graphics::ResourceSetSpecification resourceSetSpec;
        description.ResourceSetSpecification = resourceSetSpec;

        m_Pipeline = m_GraphicsDevice->CreatePipeline(description);
    }

    virtual void Unload() override
    {
        delete m_Shader;
        delete m_Pipeline;
        delete m_VertexBuffer;
    }

private:
    Nexus::Graphics::CommandList *m_CommandList = nullptr;
    Nexus::Graphics::RenderPass *m_RenderPass = nullptr;

    Nexus::Graphics::Shader *m_Shader = nullptr;
    Nexus::Graphics::Pipeline *m_Pipeline = nullptr;
    Nexus::Graphics::VertexBuffer *m_VertexBuffer = nullptr;
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