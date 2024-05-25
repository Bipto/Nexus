#include "NexusEngine.hpp"

#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus/UI/Canvas.hpp"

#include "Nexus/UI/Button.hpp"
#include "Nexus/UI/Label.hpp"
#include "Nexus/UI/PictureBox.hpp"
#include "Nexus/UI/Panel.hpp"
#include "Nexus/UI/Scrollable.hpp"
#include "Nexus/UI/ImageButton.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

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

        m_Font = new Nexus::Graphics::Font("C://Windows//Fonts//Calibri.ttf", 32, fontRange, Nexus::Graphics::FontType::Bitmap, m_GraphicsDevice);

        Nexus::Ref<Nexus::Graphics::Texture> texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);

        Nexus::UI::ImageButton *pbx = new Nexus::UI::ImageButton();
        pbx->SetPosition({5, 600});
        pbx->SetSize({300, 300});
        pbx->SetBackgroundColour({1, 0, 0, 1});
        pbx->SetTexture(texture);
        pbx->SetMargin(5, 5, 5, 5);
        pbx->SetFilter({0.15f, 0.75f, 0.15f, 1.0f});
        pbx->OnClick += [&](Nexus::UI::Control *control)
        {
            std::cout << "Image Button clicked\n";
        };

        Nexus::UI::Button *button = new Nexus::UI::Button();
        button->SetPosition({25, -15});
        button->SetSize({150, 50});
        button->SetFont(m_Font);
        button->SetText("My Button");
        button->SetBackgroundColour({0.55f, 0.55f, 0.55f, 1.0f});
        button->SetFontSize(m_Font->GetSize());
        button->SetBorderThickness(2);
        button->SetMargin(5, 5, 5, 5);
        button->OnClick += [&](Nexus::UI::Control *control)
        {
            std::cout << "Hello World!\n";
        };

        Nexus::UI::Label *label = new Nexus::UI::Label();
        label->SetPosition({125, 400});
        label->SetSize({150, 75});
        label->SetFont(m_Font);
        label->SetText("My Label\n\tSome More Text");
        label->SetBackgroundColour({1, 0, 0, 1});

        Nexus::UI::Scrollable *pnl = new Nexus::UI::Scrollable();
        pnl->SetPosition({125, 25});
        pnl->SetSize({500, 500});
        pnl->SetBackgroundColour({0.75f, 0.75f, 0.75f, 1});
        pnl->AddControl(button);
        pnl->AddControl(label);
        pnl->AddControl(pbx);
        pnl->SetScrollbarBorderThickness(1);
        pnl->SetScrollbarPadding(1.5f);
        pnl->SetScrollSpeed(45.0f);
        pnl->SetScrollbarBackgroundColour({1.0f, 0.0f, 0.0f, 1.0f});

        m_Canvas->AddControl(pnl);
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