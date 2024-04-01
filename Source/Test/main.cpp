#include "NexusEngine.hpp"

#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus/UI/Canvas.hpp"

#include "Nexus/UI/Label.hpp"
#include "Nexus/UI/Button.hpp"

#include "Nexus/UI/UIRenderer.hpp"

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
        m_Canvas = std::make_unique<Nexus::UI::Canvas>(m_GraphicsDevice, m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        std::vector<Nexus::Graphics::CharacterRange> fontRange =
            {
                {0x0020, 0x00FF}};

        m_Font = new Nexus::Graphics::Font("C://Windows//Fonts//Arial.ttf", 96, fontRange, m_GraphicsDevice);

        /* Nexus::UI::Label *label = new Nexus::UI::Label();
        label->SetPosition({0, 0});
        label->SetSize({450, 100});
        label->SetFont(font);
        label->SetText("Hello World");
        label->SetBackgroundColour({1, 0, 1, 1});
        m_Canvas->AddControl(label);

        Nexus::UI::Button *button = new Nexus::UI::Button();
        button->SetPosition({50, 150});
        button->SetSize({450, 250});
        button->SetFont(font);
        button->SetText("My Button");
        button->SetBackgroundColour({1, 0, 0, 1});

        button->OnClick += [&](Nexus::UI::Control *control)
        {
            std::cout << "Hello World!\n";
        };

        m_Canvas->AddControl(button); */

        m_UIRenderer = new Nexus::UI::UIRenderer(m_GraphicsDevice, {GetPrimaryWindow()->GetSwapchain()});
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();
        m_GraphicsDevice->BeginFrame();

        /* const auto &windowSize = GetPrimaryWindow()->GetWindowSize();
        m_Canvas->SetPosition({0, 0});
        m_Canvas->SetSize(windowSize);
        m_Canvas->SetBackgroundColour({0.42f, 0.52, 0.73f, 1.0f});
        m_Canvas->Render(); */

        m_UIRenderer->Begin();

        const auto &windowSize = GetPrimaryWindow()->GetWindowSize();
        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = windowSize.X;
        vp.Height = windowSize.Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Scissor scissor;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = windowSize.X;
        scissor.Height = windowSize.Y;

        m_UIRenderer->SetViewport(vp);
        m_UIRenderer->SetScissor(scissor);
        m_UIRenderer->DrawRectangle({0, 0, 250, 250}, {1, 0, 0, 1});
        // m_UIRenderer->DrawRectangle({0, 0, 250, 250}, {0, 0}, {1, 1}, {1.0f, 0.0f, 0.0f, 1.0f}, m_Font->GetTexture());

        m_UIRenderer->DrawCharacter('a', {0, 0, 250, 250}, {1, 0, 0, 1}, m_Font);
        m_UIRenderer->End();
        m_GraphicsDevice->SubmitCommandList(m_UIRenderer->GetCommandList());

        m_GraphicsDevice->EndFrame();
    }

    virtual void OnResize(Nexus::Point<uint32_t> size) override
    {
    }

    void CreatePipeline()
    {
    }

    virtual void Unload() override
    {
    }

private:
    std::unique_ptr<Nexus::UI::Canvas> m_Canvas = nullptr;
    Nexus::UI::UIRenderer *m_UIRenderer = nullptr;
    Nexus::Graphics::Font *m_Font = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

    spec.WindowProperties.Width = 1280;
    spec.WindowProperties.Height = 720;
    spec.WindowProperties.Resizable = true;
    spec.WindowProperties.Title = "Test Application";

    spec.SwapchainSpecification.Samples = Nexus::Graphics::SampleCount::SampleCount8;

    return new TestApplication(spec);
}