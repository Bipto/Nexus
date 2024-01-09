#include "NexusEngine.hpp"
#include "Nexus/Graphics/MeshFactory.hpp"
#include "Nexus/Graphics/Color.hpp"
#include "Nexus/Networking/Socket.hpp"

#include "Demos/ClearScreenDemo.hpp"
#include "Demos/HelloTriangle.hpp"
#include "Demos/HelloTriangleIndexed.hpp"
#include "Demos/Texturing.hpp"
#include "Demos/UniformBufferDemo.hpp"
#include "Demos/Demo3D.hpp"
#include "Demos/CameraDemo.hpp"
#include "Demos/Lighting.hpp"
#include "Demos/Models.hpp"
#include "Demos/AudioDemo.hpp"

#include "Nexus/FileSystem/FileSystem.hpp"

#include "Nexus/ImGui/ImGuiGraphicsRenderer.hpp"

#include <iostream>
#include <utility>
#include <filesystem>

struct DemoInfo
{
    std::string Name;
    Demos::Demo *(*CreationFunction)(Nexus::Application *, const std::string &name);
};

class DemoApplication : public Nexus::Application
{
public:
    DemoApplication(const Nexus::ApplicationSpecification &spec)
        : Application(spec) {}

    virtual ~DemoApplication()
    {
        delete m_CommandList;

        if (m_CurrentDemo)
        {
            delete m_CurrentDemo;
        }
    }

    virtual void Load() override
    {
        m_ImGuiRenderer = new Nexus::ImGuiUtils::ImGuiGraphicsRenderer(this);

        int size = 19;

#if defined(__ANDROID__)
        size = 48;

#endif
        auto &io = ImGui::GetIO();

        std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/fonts/roboto/roboto-regular.ttf");
        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
        m_ImGuiRenderer->RebuildFontAtlas();

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        RegisterGraphicsDemo<Demos::ClearScreenDemo>("Clear Colour");
        RegisterGraphicsDemo<Demos::HelloTriangleDemo>("Hello Triangle");
        RegisterGraphicsDemo<Demos::HelloTriangleIndexedDemo>("Hello Triangle Indexed");
        RegisterGraphicsDemo<Demos::TexturingDemo>("Texturing");
        RegisterGraphicsDemo<Demos::UniformBufferDemo>("Uniform Buffers");
        RegisterGraphicsDemo<Demos::Demo3D>("3D");
        RegisterGraphicsDemo<Demos::CameraDemo>("Camera");
        RegisterGraphicsDemo<Demos::LightingDemo>("Lighting");
        RegisterGraphicsDemo<Demos::ModelDemo>("Models");
        RegisterAudioDemo<Demos::AudioDemo>("Audio");
    }

    template <typename T>
    void RegisterGraphicsDemo(const std::string &name)
    {
        DemoInfo info;
        info.Name = name;
        info.CreationFunction = [](Nexus::Application *app, const std::string &name) -> Demos::Demo *
        {
            return new T(name, app);
        };
        m_GraphicsDemos.push_back(info);
    }

    template <typename T>
    void RegisterAudioDemo(const std::string &name)
    {
        DemoInfo info;
        info.Name = name;
        info.CreationFunction = [](Nexus::Application *app, const std::string &name) -> Demos::Demo *
        {
            return new T(name, app);
        };
        m_AudioDemos.push_back(info);
    }

    virtual void Update(Nexus::Time time) override
    {
        if (m_CurrentDemo)
            m_CurrentDemo->Update(time);
    }

    virtual void Render(Nexus::Time time) override
    {
        m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->Prepare();
        m_GraphicsDevice->BeginFrame();

        m_ImGuiRenderer->BeforeLayout(time);

        if (Nexus::Input::IsKeyPressed(Nexus::KeyCode::F11))
        {
            auto window = this->GetPrimaryWindow();
            if (window->GetCurrentWindowState() == Nexus::WindowState::Normal)
            {
                window->Maximize();
            }
            else if (window->GetCurrentWindowState() == Nexus::WindowState::Maximized)
            {
                window->Restore();
            }
        }

        {
            ImGui::Begin("Demos");

            if (m_CurrentDemo)
            {
                if (ImGui::Button("<- Back"))
                {
                    delete m_CurrentDemo;
                    m_CurrentDemo = nullptr;
                }

                // required because demo could be deleted in the previous if statement
                if (m_CurrentDemo)
                {
                    // render demo name
                    std::string label = std::string("Selected Demo - ") + m_CurrentDemo->GetName();
                    ImGui::Text("%s", label.c_str());

                    auto n = m_GraphicsDevice->GetAPIName();
                    std::string apiName = std::string("Running on : ") + std::string(m_GraphicsDevice->GetAPIName());
                    ImGui::Text("%s", apiName.c_str());

                    // render framerate
                    std::stringstream ss;
                    ss.precision(2);
                    ss << "Running at " << ImGui::GetIO().Framerate << " FPS";
                    ImGui::Text("%s", ss.str().c_str());
                    m_CurrentDemo->RenderUI();
                }
            }
            else
            {
                if (ImGui::TreeNodeEx("Graphics", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
                {
                    for (auto &pair : m_GraphicsDemos)
                    {
                        auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;

                        if (ImGui::TreeNodeEx(pair.Name.c_str(), flags))
                        {
                            if (ImGui::IsItemClicked())
                            {
                                if (m_CurrentDemo)
                                {
                                    delete m_CurrentDemo;
                                    m_CurrentDemo = nullptr;
                                }

                                m_CurrentDemo = pair.CreationFunction(this, pair.Name);
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Audio", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
                {
                    for (auto &pair : m_AudioDemos)
                    {
                        auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;

                        if (ImGui::TreeNodeEx(pair.Name.c_str(), flags))
                        {
                            if (ImGui::IsItemClicked())
                            {
                                if (m_CurrentDemo)
                                {
                                    delete m_CurrentDemo;
                                    m_CurrentDemo = nullptr;
                                }

                                m_CurrentDemo = pair.CreationFunction(this, pair.Name);
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }

        if (m_CurrentDemo)
        {
            m_CurrentDemo->Render(time);
        }
        else
        {
            m_CommandList->Begin();

            m_CommandList->SetRenderTarget({m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
            m_CommandList->ClearColorTarget(0, {0.35f,
                                                0.25f,
                                                0.42f,
                                                1.0f});

            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        m_ImGuiRenderer->AfterLayout();

        m_GraphicsDevice->EndFrame();
    }

    virtual void OnResize(Nexus::Point<uint32_t> size) override
    {
        if (m_CurrentDemo)
            m_CurrentDemo->OnResize(size);
    }

    virtual void Unload() override
    {
    }

private:
    Nexus::Graphics::CommandList *m_CommandList;
    Demos::Demo *m_CurrentDemo = nullptr;
    std::vector<DemoInfo> m_GraphicsDemos;
    std::vector<DemoInfo> m_AudioDemos;

    Nexus::ImGuiUtils::ImGuiGraphicsRenderer *m_ImGuiRenderer = nullptr;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::D3D12;

    if (arguments.size() > 1)
    {
        if (arguments[1] == "DX")
        {
            spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
        }
    }

    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;

    spec.WindowProperties.Width = 1280;
    spec.WindowProperties.Height = 720;
    spec.WindowProperties.Title = "Demo";
    spec.WindowProperties.Resizable = true;

    return new DemoApplication(spec);
}
