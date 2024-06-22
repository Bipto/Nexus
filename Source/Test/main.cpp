#include "NexusEngine.hpp"

#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus/UI/Canvas.hpp"
#include "Nexus/UI/Button.hpp"
#include "Nexus/UI/Label.hpp"
#include "Nexus/UI/PictureBox.hpp"
#include "Nexus/UI/Scrollable.hpp"
#include "Nexus/UI/ImageButton.hpp"
#include "Nexus/UI/Window.hpp"

#include "Nexus/Graphics/RoundedRectangle.hpp"

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
    explicit EditorApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
        m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, {GetPrimaryWindow()->GetSwapchain()});

        m_Texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);
    }

    virtual void Load() override
    {
        /* m_Canvas = std::make_unique<Nexus::UI::Canvas>(m_GraphicsDevice, m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        std::vector<Nexus::Graphics::CharacterRange> fontRange =
            {
                {0x0020, 0x00FF}};

        m_Font = new Nexus::Graphics::Font("resources/fonts/Roboto/Roboto-Regular.ttf", 24, fontRange, Nexus::Graphics::FontType::Bitmap, m_GraphicsDevice);

        Nexus::Ref<Nexus::Graphics::Texture> texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);

        Nexus::UI::ImageButton *pbx = new Nexus::UI::ImageButton();
        pbx->SetPosition({-150, 600});
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
        button->SetPosition({25, 15});
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
        pnl->SetScrollbarBorderThickness(1);
        pnl->SetScrollbarPadding(1.5f);
        pnl->SetScrollSpeed(45.0f);
        m_Canvas->AddControl(pnl);

        Nexus::UI::Window *wnd = new Nexus::UI::Window();
        wnd->SetPosition({25, 25});
        wnd->SetSize({350, 350});
        wnd->SetFont(m_Font);
        m_Canvas->AddControl(wnd);

        wnd->AddControl(button);
        wnd->AddControl(label);
        wnd->AddControl(pbx);
        wnd->GetScrollable()->SetScrollSpeed(50); */
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

        m_BatchRenderer->Begin(vp, scissor);

        m_BatchRenderer->DrawQuadFill({0, 0, (float)windowSize.X, (float)windowSize.Y}, {1.0f, 0.0f, 0.0f, 1.0f});

        m_BatchRenderer->DrawCircleRegionFill({500.0f, 750.0f}, 150, {1.0f, 1.0f, 1.0f, 1.0f}, 32, 0.0f, 45.0f, m_Texture, 10.0f);

        Nexus::Graphics::RoundedRectangle rrect({150.0f, 150.0f}, {450.0f, 250.0f}, 50.0f, 50.0f, 50.0f, 50.0f);

        float tilingFactor = 1.0f;

        const Nexus::Point2D<int> mousePos = Nexus::Input::GetMousePosition();

        bool containsMouse = rrect.Contains({(float)mousePos.X, (float)mousePos.Y});
        if (containsMouse)
        {
            tilingFactor = 2.5f;
        }
        m_BatchRenderer->DrawRoundedRectangleFill(rrect, {0.32f, 0.45f, 0.8f, 1.0f}, m_Texture, tilingFactor);

        if (containsMouse)
        {
            m_BatchRenderer->DrawRoundedRectangle(rrect, {0.0f, 1.0f, 0.0f, 1.0f}, 5.0f);
        }

        m_BatchRenderer->DrawQuadFill({900.0f, 150.0f, 250.0f, 250.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, m_Texture, 0.1f);

        m_BatchRenderer->End();

        m_GraphicsDevice->EndFrame();
    }

    virtual void OnResize(Nexus::Point2D<uint32_t> size) override
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

    Nexus::Ref<Nexus::Graphics::Texture> m_Texture = nullptr;

    Nexus::Graphics::BatchRenderer *m_BatchRenderer = nullptr;
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