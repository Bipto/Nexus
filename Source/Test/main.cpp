#include "NexusEngine.hpp"

#include "Nexus/Graphics/Font.hpp"

#include "Nexus/Renderer/BatchRenderer.hpp"

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
        m_CommandList = m_GraphicsDevice->CreateCommandList();

        /* Nexus::Graphics::FramebufferSpecification spec;
        spec.ColorAttachmentSpecification.Attachments =
            {
                {Nexus::Graphics::TextureFormat::RGBA8}};
        spec.Width = 1280;
        spec.Height = 720;
        m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec); */

        Nexus::WindowSpecification windowSpec;
        windowSpec.Width = 500;
        windowSpec.Height = 500;
        windowSpec.Resizable = false;
        windowSpec.Title = "Second Window";

        Nexus::Graphics::SwapchainSpecification swapchainSpec;
        swapchainSpec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

        /* m_Window2 = this->CreateApplicationWindow(windowSpec, swapchainSpec);
        m_Window2->CreateSwapchain(m_GraphicsDevice, swapchainSpec);
        m_Window2->GetSwapchain()->Initialise(); */

        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("resources/shaders/basic.glsl", Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());
        m_Texture = m_GraphicsDevice->CreateTexture("resources/textures/brick.jpg");

        Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
        m_QuadMesh = factory.CreateSprite();

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(TestUniforms);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(uniformBufferDesc, nullptr);

        CreatePipeline();
        m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});

        std::vector<Nexus::Graphics::CharacterRange> fontRange =
            {
                {0x0020, 0x00FF}};

        m_Font = new Nexus::Graphics::Font("resources/fonts/roboto/Roboto-Regular.ttf", fontRange, m_GraphicsDevice);
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_TestUniforms.Transform = glm::mat4(1.0f);
        void *dst = m_UniformBuffer->Map();
        memcpy(dst, &m_TestUniforms, sizeof(m_TestUniforms));
        m_UniformBuffer->Unmap();

        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

        m_CommandList->Begin();

        Nexus::Graphics::RenderTarget swapchainTarget(m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->ClearColorTarget(0, {0.35f, 0.42f, 0.63f, 1.0f});

        m_ResourceSet->WriteTexture(m_Texture, 0);
        m_ResourceSet->WriteUniformBuffer(m_UniformBuffer, 0);
        m_CommandList->SetResourceSet(m_ResourceSet);

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
        vp.Height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        m_CommandList->SetViewport(vp);

        Nexus::Graphics::Rectangle scissor;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
        scissor.Height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;
        m_CommandList->SetScissor(scissor);

        /* if (!m_Window2->IsClosing())
        {
            m_Window2->GetSwapchain()->Prepare();

            Nexus::Graphics::RenderTarget otherSwapchainTarget(m_Window2->GetSwapchain());
            m_CommandList->SetRenderTarget(otherSwapchainTarget);
            m_CommandList->ClearColorTarget(0, {0.0f, 1.0f, 0.0f, 1.0f});
        } */

        m_CommandList->SetVertexBuffer(m_QuadMesh->GetVertexBuffer());
        m_CommandList->SetIndexBuffer(m_QuadMesh->GetIndexBuffer());
        m_CommandList->DrawIndexed(6, 0);

        m_CommandList->End();
        m_GraphicsDevice->SubmitCommandList(m_CommandList);

        glm::mat4 projection = glm::ortho<float>(0.0f, m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X, m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y, 0.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 mvp = projection * view;

        m_BatchRenderer->Begin(mvp);
        m_BatchRenderer->DrawQuadFill({100.0f, 100.0f}, {250.0f, 250.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
        m_BatchRenderer->DrawString("Hello\nWorld!\tThis line is tabbed across", {0.0f, 0.0f}, 22, {0.2f, 0.2f, 0.2f, 1.0f}, m_Font);
        m_BatchRenderer->DrawLine({0.0f, 0.0f}, {GetPrimaryWindow()->GetWindowSize().X, GetPrimaryWindow()->GetWindowSize().Y}, {1.0f, 1.0f, 1.0f, 1.0f}, 2.0f);
        m_BatchRenderer->DrawLine({0.0f, GetPrimaryWindow()->GetWindowSize().Y}, {GetPrimaryWindow()->GetWindowSize().X, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 5.0f);
        m_BatchRenderer->DrawCircle({500.0f, 500.0f}, 96.0f, {0.0f, 1.0f, 0.0f, 1.0f}, 48, 2.0f);
        m_BatchRenderer->DrawCircleFill({700.0f, 500.0f}, 128.0f, {0.0f, 0.0f, 1.0f, 1.0f}, 48);
        m_BatchRenderer->DrawQuad({900.0f, 100.0f}, {500.0f, 500.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, 8.0f);

        std::string text = "abc\nHello";
        auto fontSize = 272;
        auto size = m_Font->MeasureString(text, fontSize);
        m_BatchRenderer->DrawQuadFill({700.0f, 200.0f}, {700 + size.X, 200 + size.Y}, {1.0f, 0.0f, 0.0f, 1.0f});
        m_BatchRenderer->DrawString(text, {700.0f, 200.0f}, fontSize, {0.2f, 0.2f, 0.2f, 1.0f}, m_Font);

        m_BatchRenderer->End();

        /* if (!m_Window2->IsClosing())
        {
            m_Window2->GetSwapchain()->SwapBuffers();
        } */
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        m_BatchRenderer->Resize();
    }

    void CreatePipeline()
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
        description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;

        Nexus::Graphics::TextureResourceBinding textureBinding;
        textureBinding.Name = "texSampler";
        textureBinding.Slot = 0;

        Nexus::Graphics::UniformResourceBinding uniformBufferBinding;
        uniformBufferBinding.Binding = 0;
        uniformBufferBinding.Name = "Transform";
        uniformBufferBinding.Buffer = m_UniformBuffer;

        Nexus::Graphics::ResourceSetSpecification resourceSetSpec;
        resourceSetSpec.TextureBindings = {textureBinding};
        resourceSetSpec.UniformResourceBindings = {uniformBufferBinding};
        description.ResourceSetSpecification = resourceSetSpec;

        description.Target = Nexus::Graphics::RenderTarget(m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        m_Pipeline = m_GraphicsDevice->CreatePipeline(description);
        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
    }

    virtual void Unload() override
    {
        delete m_Shader;
        delete m_Pipeline;
        delete m_CommandList;
        delete m_Texture;
        delete m_UniformBuffer;
    }

private:
    Nexus::Graphics::CommandList *m_CommandList = nullptr;

    Nexus::Graphics::Shader *m_Shader = nullptr;
    Nexus::Graphics::Pipeline *m_Pipeline = nullptr;

    Nexus::Graphics::ResourceSet *m_ResourceSet = nullptr;
    Nexus::Graphics::Texture *m_Texture = nullptr;
    Nexus::Graphics::UniformBuffer *m_UniformBuffer = nullptr;
    TestUniforms m_TestUniforms;

    Nexus::Graphics::Framebuffer *m_Framebuffer = nullptr;
    Nexus::Window *m_Window2 = nullptr;

    Nexus::Graphics::Font *m_Font = nullptr;
    Nexus::Graphics::Mesh *m_QuadMesh = nullptr;

    Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::D3D11;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

    spec.WindowProperties.Width = 1280;
    spec.WindowProperties.Height = 720;
    spec.WindowProperties.Resizable = true;
    spec.WindowProperties.Title = "Test Application";

    spec.SwapchainSpecification.Samples = 16;

    return new TestApplication(spec);
}