#include "NexusEngine.hpp"
#include "Core/Graphics/MeshFactory.hpp"
#include "Core/Graphics/Color.hpp"
#include "Core/Networking/Socket.hpp"

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

#include <iostream>
#include <utility>

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

    virtual void Load() override
    {
        auto &io = ImGui::GetIO();
        /* io.FontDefault = io.Fonts->AddFontFromFileTTF(
            "Resources/Fonts/Roboto/Roboto-Regular.ttf", 18); */

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        Nexus::Graphics::RenderPassSpecification spec;
        spec.ColorLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        spec.StencilDepthLoadOperation = Nexus::Graphics::LoadOperation::Clear;
        spec.Framebuffer = nullptr;
        m_RenderPass = m_GraphicsDevice->CreateRenderPass(spec);

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
                    ImGui::Text(label.c_str());

                    auto n = m_GraphicsDevice->GetAPIName();
                    std::string apiName = std::string("Running on : ") + std::string(m_GraphicsDevice->GetAPIName());
                    ImGui::Text(apiName.c_str());

                    // render framerate
                    std::stringstream ss;
                    ss.precision(2);
                    ss << "Running at " << ImGui::GetIO().Framerate << " FPS";
                    ImGui::Text(ss.str().c_str());
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
                0.0f,
                0.0f,
                0.0f,
                1.0f};
            beginInfo.Framebuffer = nullptr;

            m_CommandList->Begin();
            m_CommandList->BeginRenderPass(beginInfo);
            m_CommandList->EndRenderPass();
            m_CommandList->End();
            m_GraphicsDevice->SubmitCommandList(m_CommandList);
        }
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        if (m_CurrentDemo)
            m_CurrentDemo->OnResize(size);

        m_GraphicsDevice->Resize(size);
    }

    virtual void Unload() override
    {
    }

private:
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
    Nexus::Ref<Nexus::Graphics::RenderPass> m_RenderPass;

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
