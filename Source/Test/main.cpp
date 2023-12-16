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

        Nexus::Graphics::FramebufferSpecification spec;
        spec.ColorAttachmentSpecification.Attachments =
            {
                {Nexus::Graphics::TextureFormat::RGBA8}};
        spec.Width = 1280;
        spec.Height = 720;
        m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec);

        Nexus::WindowSpecification windowSpec;
        windowSpec.Width = 500;
        windowSpec.Height = 500;
        windowSpec.Resizable = false;
        windowSpec.Title = "Second Window";

        Nexus::Graphics::SwapchainSpecification swapchainSpec;
        swapchainSpec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

        m_Window2 = this->CreateApplicationWindow(windowSpec, swapchainSpec);
        m_Window2->CreateSwapchain(m_GraphicsDevice, swapchainSpec);
        m_Window2->GetSwapchain()->Initialise();
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

        m_CommandList->Begin();

        Nexus::Graphics::RenderTarget swapchainTarget(m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());
        m_CommandList->SetRenderTarget(swapchainTarget);
        m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});

        if (!m_Window2->IsClosing())
        {
            m_Window2->GetSwapchain()->Prepare();

            Nexus::Graphics::RenderTarget otherSwapchainTarget(m_Window2->GetSwapchain());
            m_CommandList->SetRenderTarget(otherSwapchainTarget);
            m_CommandList->ClearColorTarget(0, {0.0f, 1.0f, 0.0f, 1.0f});
        }

        Nexus::Graphics::RenderTarget framebufferTarget(m_Framebuffer);
        m_CommandList->SetRenderTarget(framebufferTarget);
        m_CommandList->ClearColorTarget(0, {0.0f, 1.0f, 0.0f, 1.0f});

        m_CommandList->End();
        m_GraphicsDevice->SubmitCommandList(m_CommandList);

        if (!m_Window2->IsClosing())
        {
            m_Window2->GetSwapchain()->SwapBuffers();
        }
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        /* CreatePipeline(size);
        m_BatchRenderer->Resize(); */
    }

    void CreatePipeline(Nexus::Point<int> size)
    {
        /* if (m_Pipeline)
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
        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline); */
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
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::Vulkan;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

    spec.WindowProperties.Width = 1280;
    spec.WindowProperties.Height = 720;
    spec.WindowProperties.Resizable = true;
    spec.WindowProperties.Title = "Test Application";

    spec.SwapchainSpecification.Samples = 1;

    return new TestApplication(spec);
}