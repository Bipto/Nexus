#include "NexusEngine.hpp"

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus-Core/Graphics/CatmullRom.hpp"
#include "Nexus-Core/Graphics/RoundedRectangle.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"

#include "Nexus-Core/UI/Button.hpp"
#include "Nexus-Core/UI/Canvas.hpp"
#include "Nexus-Core/UI/Label.hpp"
#include "Nexus-Core/UI/Panel.hpp"
#include "Nexus-Core/UI/PictureBox.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

class Sandbox : public Nexus::Application
{
  public:
    explicit Sandbox(const Nexus::ApplicationSpecification &spec) : Nexus::Application(spec)
    {
    }

    virtual void Load() override
    {
        m_Texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);
        m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, Nexus::Graphics::RenderTarget{m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});

        rrect = Nexus::Graphics::RoundedRectangle(100, 100, 500, 500, 25, 25, 25, 25);

        m_Canvas = std::make_unique<Nexus::UI::Canvas>(m_GraphicsDevice, m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        std::vector<Nexus::Graphics::CharacterRange> fontRange = {{0x0020, 0x00FF}};

        m_Font = new Nexus::Graphics::Font("resources/Fonts/JETBRAINSMONO-REGULAR.TTF", 18, fontRange, Nexus::Graphics::FontType::Bitmap, m_GraphicsDevice);
        Nexus::Ref<Nexus::Graphics::Texture> texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);

        Nexus::UI::Label *lbl = new Nexus::UI::Label();
        lbl->SetFont(m_Font);
        lbl->SetLocalPosition({10, 10});
        lbl->SetSize({1200, 400});
        lbl->SetText("Hello World!Even more text...\n\tHopefully this second line is longer than the first:)");
        lbl->SetFontSize(m_Font->GetSize());
        lbl->SetAutoSize(true);
        lbl->SetCornerRounding(10.0f);

        Nexus::UI::Button *btn = new Nexus::UI::Button();
        btn->SetFont(m_Font);
        btn->SetLocalPosition({10, 175});
        btn->SetSize({1200, 400});
        btn->SetText("My Button");
        btn->SetFontSize(m_Font->GetSize());
        btn->SetAutoSize(true);
        btn->SetCornerRounding(10.0f);
        btn->SetHoveredColour({0.0f, 0.25f, 0.45f, 1.0f});
        btn->SetBorderThickness(1.0f);
        btn->OnMouseClick += [&](Nexus::UI::Control *ctrl) { std::cout << "Button clicked\n"; };

        Nexus::UI::PictureBox *pbx = new Nexus::UI::PictureBox();
        pbx->SetLocalPosition({25, 275});
        pbx->SetSize({350, 350});
        pbx->SetFontSize(m_Font->GetSize());
        pbx->SetCornerRounding(10.0f);
        pbx->SetBorderThickness(0.0f);
        pbx->SetTexture(m_Texture);
        pbx->SetAutoSize(true);
        pbx->SetPadding(Nexus::UI::Padding(20.0f));

        Nexus::UI::Padding padding;
        padding.Left = 50.0f;
        padding.Right = 50.0f;
        padding.Top = 50.0f;
        padding.Bottom = 50.0f;
        lbl->SetPadding(padding);

        Nexus::UI::Panel *pnl = new Nexus::UI::Panel();
        pnl->SetLocalPosition({100, 100});
        pnl->SetSize({500, 500});
        pnl->SetBackgroundColour({0.0f, 0.0f, 1.0f, 1.0f});
        // pnl->AddControl(btn);
        // pnl->AddControl(lbl);
        pnl->AddControl(pbx);
        pnl->SetCornerRounding(35.0f);
        m_Canvas->AddControl(pnl);

        Nexus::UI::Panel *pnl2 = new Nexus::UI::Panel();
        pnl2->SetLocalPosition({200, 200});
        pnl2->SetSize({500, 500});
        pnl2->SetCornerRounding(25.0f);
        pnl2->SetBackgroundColour({1.0f, 0.0f, 1.0f, 1.0f});
        // pnl->AddControl(pnl2);
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();

        const auto &windowSize = GetPrimaryWindow()->GetWindowSize();
        m_Canvas->SetPosition({0, 0});
        m_Canvas->SetSize(windowSize);
        m_Canvas->SetBackgroundColour({0.42f, 0.52, 0.73f, 1.0f});
        m_Canvas->Render();
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

    Nexus::Graphics::RoundedRectangle rrect;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

    spec.WindowProperties.Width = 1280;
    spec.WindowProperties.Height = 720;
    spec.WindowProperties.Resizable = true;
    spec.WindowProperties.Title = "Sandbox";

    spec.SwapchainSpecification.Samples = Nexus::Graphics::SampleCount::SampleCount8;

    return new Sandbox(spec);
}