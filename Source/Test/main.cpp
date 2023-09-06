#include "NexusEngine.hpp"

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
        Nexus::Graphics::TextureResourceBinding textureBinding;
        textureBinding.Slot = 0;
        textureBinding.Name = "texSampler";

        Nexus::Graphics::UniformResourceBinding uniformBinding;
        uniformBinding.Binding = 0;
        uniformBinding.Name = "transformBuffer";
        uniformBinding.Size = sizeof(TestUniforms);

        Nexus::Graphics::ResourceSetSpecification resourceSetSpec;
        resourceSetSpec.TextureBindings = {textureBinding};
        resourceSetSpec.UniformResourceBindings = {uniformBinding};

        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(resourceSetSpec);

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        Nexus::Graphics::RenderPassSpecification spec;
        spec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        spec.StencilDepthLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());

        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/basic.glsl", Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(TestUniforms);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(uniformBufferDesc, nullptr);

        CreatePipeline({GetWindowSize().X,
                        GetWindowSize().Y});

        Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
        m_Mesh = factory.CreateSprite();

        m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->BeginFrame();
        Nexus::Graphics::RenderPassBeginInfo beginInfo;
        beginInfo.ClearColorValue = {
            1.0f,
            0.0f,
            0.0f,
            1.0f};
        beginInfo.ClearDepthStencilValue.Depth = 1.0f;

        m_ResourceSet->UpdateTexture(
            m_Texture,
            0);

        m_TestUniforms.Transform = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), {0, 0, 1});
        m_UniformBuffer->SetData(&m_TestUniforms, sizeof(m_TestUniforms), 0);

        m_CommandList->Begin();
        m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->WriteTexture(m_Texture, m_ResourceSet, 0);
        m_CommandList->WriteUniformBuffer(m_UniformBuffer, m_ResourceSet, 0);
        m_CommandList->SetResources(m_ResourceSet);
        m_CommandList->SetVertexBuffer(m_Mesh.GetVertexBuffer());
        m_CommandList->SetIndexBuffer(m_Mesh.GetIndexBuffer());

        auto indexCount = m_Mesh.GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
        m_CommandList->DrawIndexed(indexCount, 0);
        m_CommandList->EndRenderPass();
        m_CommandList->End();
        m_GraphicsDevice->SubmitCommandList(m_CommandList);

        ImGui::ShowDemoWindow();

        m_GraphicsDevice->EndFrame();
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        m_GraphicsDevice->Resize(size);
        CreatePipeline(size);
    }

    void CreatePipeline(Nexus::Point<int> size)
    {
        Nexus::Graphics::PipelineDescription description;
        description.Shader = m_Shader;
        description.RenderPass = m_RenderPass;
        description.Viewport.X = 0;
        description.Viewport.Y = 0;
        description.Viewport.Width = size.X;
        description.Viewport.Height = size.Y;
        description.RasterizerStateDescription.ScissorRectangle =
            {
                0, 0, size.X, size.Y};
        description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
        description.ResourceSet = m_ResourceSet;

        m_Pipeline = m_GraphicsDevice->CreatePipeline(description);
    }

    virtual void Unload() override
    {
    }

private:
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
    Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;
    Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;

    Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
    Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
    Nexus::Graphics::Mesh m_Mesh;

    Nexus::Ref<Nexus::Graphics::Texture> m_Texture;
    Nexus::Ref<Nexus::Graphics::UniformBuffer> m_UniformBuffer;

    TestUniforms m_TestUniforms;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::Vulkan;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = true;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new TestApplication(spec);
}