#include "NexusEngine.hpp"

#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus/UI/Canvas.hpp"

#include "Nexus/UI/Button.hpp"
#include "Nexus/UI/Label.hpp"

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

class EditorApplication : public Nexus::Application
{
public:
    EditorApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
    }

    virtual void Load() override
    {
        m_Canvas = std::make_unique<Nexus::UI::Canvas>(m_GraphicsDevice, m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        std::vector<Nexus::Graphics::CharacterRange> fontRange =
            {
                {0x0020, 0x00FF}};

        m_Font = new Nexus::Graphics::Font("C://Windows//Fonts//Calibri.ttf", 32, fontRange, Nexus::Graphics::FontType::SDF, m_GraphicsDevice);

        Nexus::UI::Button *button = new Nexus::UI::Button();
        button->SetPosition({150, 150});
        button->SetSize({500, 500});
        button->SetFont(m_Font);
        button->SetText("My Button\n\tSome More Text");
        button->SetBackgroundColour({0.75f, 0.75f, 0.75f, 1.0f});
        button->SetFontSize(m_Font->GetSize());
        button->SetBorderThickness(2);
        button->SetMargin(5, 5, 5, 5);
        button->OnClick += [&](Nexus::UI::Control *control)
        {
            std::cout << "Hello World!\n";
        };
        m_Canvas->AddControl(button);

        Nexus::UI::Label *label = new Nexus::UI::Label();
        label->SetPosition({0, 0});
        label->SetSize({150, 75});
        label->SetFont(m_Font);
        label->SetText("My Label");
        label->SetBackgroundColour({1, 0, 0, 1});
        m_Canvas->AddControl(label);
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();
        m_GraphicsDevice->BeginFrame();

        const auto &windowSize = GetPrimaryWindow()->GetWindowSize();
        m_Canvas->SetPosition({0, 0});
        m_Canvas->SetSize(windowSize);
        m_Canvas->SetBackgroundColour({0.42f, 0.52, 0.73f, 1.0f});
        m_Canvas->Render();

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

    return new EditorApplication(spec);
}