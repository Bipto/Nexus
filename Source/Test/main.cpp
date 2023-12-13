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
        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("resources/shaders/basic.glsl", Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout());

        Nexus::Graphics::RenderPassSpecification renderPassSpec;
        renderPassSpec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        m_RenderPass = m_GraphicsDevice->CreateRenderPass(renderPassSpec, GetPrimaryWindow()->GetSwapchain());

        CreatePipeline(GetWindowSize());

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(TestUniforms);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(uniformBufferDesc, nullptr);

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        // offscreen rendering
        Nexus::Graphics::FramebufferSpecification fbSpec;
        fbSpec.Width = 500;
        fbSpec.Height = 500;
        fbSpec.ColorAttachmentSpecification =
            {
                Nexus::Graphics::TextureFormat::Color};

        Nexus::Graphics::RenderPassSpecification rpSpec;
        rpSpec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;

        auto pair = m_GraphicsDevice->CreateRenderPassAndFramebuffer(rpSpec, fbSpec);
        m_Framebuffer = pair.first;
        m_OffscreenRenderPass = pair.second;

        m_Texture = m_GraphicsDevice->CreateTexture("resources/textures/brick.jpg");

        /* Nexus::WindowProperties props;
        props.Title = "Second Window";
        props.Resizable = false;
        m_Window2 = CreateApplicationWindow(props);
        m_Window2->CreateSwapchain(m_GraphicsDevice, Nexus::Graphics::VSyncState::Enabled);
        m_Window2->GetSwapchain()->Initialise();

        Nexus::Graphics::RenderPassSpecification spec;
        spec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        m_Window2RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_Window2->GetSwapchain()); */

        std::vector<Nexus::Graphics::CharacterRange> fontRange =
            {
                {0x0020, 0x00FF}};

        // m_Font = new Nexus::Graphics::Font("resources/fonts/Roboto/Roboto-Regular.ttf", fontRange, m_GraphicsDevice);
        m_Font = new Nexus::Graphics::Font("C://Windows//Fonts//Arial.ttf", fontRange, m_GraphicsDevice);

        Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
        m_QuadMesh = factory.CreateSprite();

        m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, m_RenderPass);
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        /* if (m_Window2RenderPass->IsValid())
        {
            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                0.45f,
                0.32f,
                0.55f,
                1.0f};

            m_Window2->GetSwapchain()->Prepare();
            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(m_Window2RenderPass, beginInfo);
            {
            }
            m_CommandList->EndRenderPass();
            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);
            m_Window2->GetSwapchain()->SwapBuffers();
        } */

        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();
        m_TestUniforms.Transform = glm::mat4(1.0f);
        // m_UniformBuffer->SetData(&m_TestUniforms, sizeof(m_TestUniforms), 0);

        /* void *buffer = m_UniformBuffer->Map();
        memcpy(buffer, &m_TestUniforms, sizeof(m_TestUniforms));
        m_UniformBuffer->Unmap();

        m_ResourceSet->WriteUniformBuffer(m_UniformBuffer, 0);
        m_ResourceSet->WriteTexture(m_Font->GetTexture(), 0);

        m_GraphicsDevice->BeginFrame();
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
            m_CommandList->SetVertexBuffer(m_QuadMesh->GetVertexBuffer());
            m_CommandList->SetIndexBuffer(m_QuadMesh->GetIndexBuffer());
            m_CommandList->DrawIndexed(6, 0);
        }
        m_CommandList->EndRenderPass();
        m_CommandList->End();
        m_GraphicsDevice->EndFrame();

        m_GraphicsDevice->SubmitCommandList(m_CommandList);*/

        Nexus::Graphics::RenderPassBeginInfo beginInfo;
        beginInfo.ClearColorValue = {
            0.45f,
            0.32f,
            0.55f,
            1.0f};
        beginInfo.ClearDepthStencilValue = {1.0f, 0};

        uint32_t width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
        uint32_t height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;

        glm::mat4 projection = glm::ortho<float>(0.0f, width, height, 0.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 mvp = projection * view;

        m_BatchRenderer->Begin(beginInfo, mvp);
        // m_BatchRenderer->DrawRectangle({0, 0}, {width, height}, {1.0f, 1.0f, 1.0f, 1.0f}, m_Font->GetTexture());
        //  m_BatchRenderer->DrawRectangle({50.0f, 50.0f}, {200.0f, 200.0f}, {0.7f, 0.3f, 0.25f, 1.0f});
        //  m_BatchRenderer->DrawRectangle({500.0f, 500.0f}, {700.0f, 700.0f}, {0.4f, 0.6f, 0.15f, 1.0f});
        //  m_BatchRenderer->DrawRectangle({700.0f, 50.0f}, {1000.0f, 350.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, m_Texture);

        m_BatchRenderer->DrawRectangle({0, 0}, {width, height}, {1.0f, 1.0f, 1.0f, 1.0f});
        m_BatchRenderer->DrawString("Hello World!", {50.0f, 50.0f}, 1, {1.0f, 0.0f, 0.0f, 1.0f}, m_Font);
        // m_BatchRenderer->DrawCharacter('!', {0.0f, 0.0f}, 32, {1.0f, 0.0f, 0.0f, 1.0f}, m_Font);
        m_BatchRenderer->End();
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        CreatePipeline(size);
        m_BatchRenderer->Resize();
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

        m_Pipeline = m_GraphicsDevice->CreatePipeline(description);
        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
    }

    virtual void Unload() override
    {
        delete m_Shader;
        delete m_Pipeline;
        delete m_CommandList;
        delete m_RenderPass;
        delete m_Texture;
        delete m_UniformBuffer;
    }

private:
    Nexus::Graphics::CommandList *m_CommandList = nullptr;
    Nexus::Graphics::RenderPass *m_RenderPass = nullptr;

    Nexus::Graphics::Shader *m_Shader = nullptr;
    Nexus::Graphics::Pipeline *m_Pipeline = nullptr;

    Nexus::Graphics::ResourceSet *m_ResourceSet = nullptr;
    Nexus::Graphics::Texture *m_Texture = nullptr;

    Nexus::Graphics::UniformBuffer *m_UniformBuffer = nullptr;
    TestUniforms m_TestUniforms;

    Nexus::Graphics::Framebuffer *m_Framebuffer = nullptr;
    Nexus::Graphics::RenderPass *m_OffscreenRenderPass = nullptr;

    Nexus::Window *m_Window2 = nullptr;
    Nexus::Graphics::RenderPass *m_Window2RenderPass = nullptr;

    Nexus::Graphics::Font *m_Font = nullptr;
    Nexus::Graphics::Mesh *m_QuadMesh = nullptr;

    Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = false;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new TestApplication(spec);
}