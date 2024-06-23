#include "NexusEngine.hpp"

#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

#include "Nexus/UI/Canvas.hpp"

#include "Nexus/Graphics/RoundedRectangle.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

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
    explicit EditorApplication(const Nexus::ApplicationSpecification &spec)
        : Nexus::Application(spec)
    {
        m_BatchRenderer = new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, {GetPrimaryWindow()->GetSwapchain()});

        m_Texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);
    }

    virtual void Load() override
    {
        m_Canvas = std::make_unique<Nexus::UI::Canvas>(m_GraphicsDevice, m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain());

        std::vector<Nexus::Graphics::CharacterRange> fontRange =
            {
                {0x0020, 0x00FF}};

        m_Font = new Nexus::Graphics::Font("resources/Fonts/JETBRAINSMONO-REGULAR.TTF", 8, fontRange, Nexus::Graphics::FontType::Bitmap, m_GraphicsDevice);
        Nexus::Ref<Nexus::Graphics::Texture> texture = m_GraphicsDevice->CreateTexture(Nexus::FileSystem::GetFilePathAbsolute("resources/textures/brick.jpg"), false);

        Nexus::UI::Label *lbl = new Nexus::UI::Label();
        lbl->SetFont(m_Font);
        lbl->SetPosition({10, 10});
        lbl->SetSize({1200, 400});
        lbl->SetText("Hello World!Even more text...\n\tHopefully this second line is longer than the first:)");
        lbl->SetFontSize(m_Font->GetSize());
        // lbl->SetAutoSize(true);
        m_Canvas->AddControl(lbl);
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