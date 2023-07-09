#include "NexusEngine.h"
#include "Core/Graphics/MeshFactory.h"
#include "Core/Graphics/Color.h"
#include "Core/Networking/Socket.h"

#include "Demos/ClearScreenDemo.h"
#include "Demos/HelloTriangle.h"
#include "Demos/HelloTriangleIndexed.h"
#include "Demos/Texturing.h"
#include "Demos/UniformBufferDemo.h"
#include "Demos/Demo3D.h"
#include "Demos/CameraDemo.h"

#include <iostream>
#include <utility>

const char *vertexShaderSource =
    "#version 300 es\n"
    "layout(std140) uniform Camera\n"
    "{\n"
    "mat4 u_View;\n"
    "mat4 u_Projection;\n"
    "}_28;\n"
    "layout(std140) uniform RenderInfo\n"
    "{\n"
    "mat4 u_Transform;\n"
    "vec3 u_Color;\n"
    "}_23;\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "out vec3 OutColor;\n"
    "void main()\n"
    "{\n"
    "gl_Position = _28.u_Projection * _28.u_View * _23.u_Transform * vec4(aPos, 1.0);\n"
    "TexCoord = aTexCoord;\n"
    "OutColor = _23.u_Color;\n"
    "}";

std::string fragmentShaderSource =
    "#version 300 es\n"
    "precision mediump float;\n"
    "precision highp int;\n"
    "layout (location = 0) out vec4 FragColor;\n"
    "in highp vec2 TexCoord;\n"
    "in highp vec3 OutColor;\n"
    "uniform sampler2D texSampler;\n"
    "void main()\n"
    "{\n"
    "FragColor = texture(texSampler, TexCoord) * vec4(OutColor, 1.0);\n"
    "}";

struct DemoInfo
{
    std::string Name;
    Demos::Demo *(*CreationFunction)(Nexus::Application *, const std::string &name);
};

class DemoApplication : public Nexus::Application
{
public:
    DemoApplication(const Nexus::ApplicationSpecification &spec) : Application(spec) {}

    virtual void Load() override
    {
        auto &io = ImGui::GetIO();
        /* io.FontDefault = io.Fonts->AddFontFromFileTTF(
            "Resources/Fonts/Roboto/Roboto-Regular.ttf", 18); */

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        RegisterDemo<Demos::ClearScreenDemo>("Clear Colour");
        RegisterDemo<Demos::HelloTriangleDemo>("Hello Triangle");
        RegisterDemo<Demos::HelloTriangleIndexedDemo>("Hello Triangle Indexed");
        RegisterDemo<Demos::TexturingDemo>("Texturing");
        RegisterDemo<Demos::UniformBufferDemo>("Uniform Buffers");
        RegisterDemo<Demos::Demo3D>("3D");
        RegisterDemo<Demos::CameraDemo>("Camera");
    }

    template <typename T>
    void RegisterDemo(const std::string &name)
    {
        DemoInfo info;
        info.Name = name;
        info.CreationFunction = [](Nexus::Application *app, const std::string &name) -> Demos::Demo *
        {
            return new T(name, app);
        };
        m_AvailableDemos.push_back(info);
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
                    for (auto &pair : m_AvailableDemos)
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
            Nexus::Graphics::CommandListBeginInfo beginInfo{};
            beginInfo.ClearValue = {
                0.0f,
                0.0f,
                0.0f,
                1.0f};

            m_CommandList->Begin(beginInfo);
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
    Demos::Demo *m_CurrentDemo = nullptr;
    std::vector<DemoInfo> m_AvailableDemos;
};

int main(int argc, char **argv)
{
    Nexus::ApplicationSpecification spec;
    spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::OpenGL;
    spec.AudioAPI = Nexus::Audio::AudioAPI::OpenAL;
    spec.ImGuiActive = true;
    spec.VSyncState = Nexus::Graphics::VSyncState::Enabled;
    spec.UpdatesPerSecond = 1;
    spec.RendersPerSecond = 30;

    std::vector<std::string> arguments;
    for (int i = 0; i < argc; i++)
    {
        std::string argument = std::string(argv[i]);
        arguments.push_back(argument);
    }

    if (arguments.size() > 1)
        if (arguments[1] == std::string("DX"))
            spec.GraphicsAPI = Nexus::Graphics::GraphicsAPI::DirectX11;

    Nexus::Init(argc, argv);

    DemoApplication *app = new DemoApplication(spec);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
}