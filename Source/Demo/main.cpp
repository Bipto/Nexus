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
        delete m_RenderPass;

        if (m_CurrentDemo)
        {
            delete m_CurrentDemo;
        }
    }

    virtual void Load() override
    {
        auto &io = ImGui::GetIO();

        int size = 19;

#if defined(__ANDROID__)
        size = 48;

#endif

        std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/fonts/roboto/roboto-regular.ttf");
        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
        m_ImGuiRenderer->RebuildFontAtlas();

        Nexus::Graphics::RenderPassSpecification spec;
        spec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        spec.StencilDepthLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec, m_GraphicsDevice->GetSwapchain());

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
        m_GraphicsDevice->BeginFrame();

        if (Nexus::Input::IsKeyPressed(Nexus::KeyCode::F11))
        {
            auto window = this->GetWindow();
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
            Nexus::Graphics::RenderPassBeginInfo beginInfo{};
            beginInfo.ClearColorValue = {
                0.35f,
                0.25f,
                0.42f,
                1.0f};

            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(m_RenderPass, beginInfo);
            m_CommandList->EndRenderPass();
            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }

        m_GraphicsDevice->EndFrame();
    }

    virtual void
    OnResize(Nexus::Point<int> size) override
    {
        if (m_CurrentDemo)
            m_CurrentDemo->OnResize(size);
    }

    virtual void Unload() override
    {
    }

private:
    Nexus::Graphics::CommandList *m_CommandList;
    Nexus::Graphics::RenderPass *m_RenderPass;

    Demos::Demo *m_CurrentDemo = nullptr;

    std::vector<DemoInfo> m_GraphicsDemos;
    std::vector<DemoInfo> m_AudioDemos;
};

Nexus::Application *Nexus::CreateApplication(const CommandLineArguments &arguments)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;

    if (arguments.size() > 1)
    {
        if (arguments[1] == "DX")
        {
            spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::DirectX11;
        }
    }

    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = true;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;

    return new DemoApplication(spec);
}
