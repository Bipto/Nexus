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

/* struct alignas(16) VB_UNIFORM_CAMERA
{
    glm::mat4 View;
    glm::mat4 Projection;
};

struct alignas(16) VB_UNIFORM_RENDERINFO
{
    glm::mat4 Transform;
    glm::vec3 Color;
}; */

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
        /* Nexus::Graphics::VertexBufferLayout layout =
            {
                {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD", 0},
                {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD", 1}};

        Nexus::Graphics::FramebufferSpecification spec;
        spec.Width = 500;
        spec.Height = 500;
        spec.ColorAttachmentSpecification = {Nexus::Graphics::TextureFormat::RGBA8};
        m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec);

        m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");

#ifndef __EMSCRIPTEN__
        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/demo_shader.glsl", layout);
#else
        m_Shader = m_GraphicsDevice->CreateShaderFromSource(vertexShaderSource, fragmentShaderSource, layout);
#endif

        Nexus::Graphics::UniformResourceBinding cameraUniformBinding;
        cameraUniformBinding.Binding = 0;
        cameraUniformBinding.Name = "Camera";
        cameraUniformBinding.Size = sizeof(VB_UNIFORM_CAMERA);
        m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBinding);
        m_CameraUniformBuffer->BindToShader(m_Shader);

        Nexus::Graphics::UniformResourceBinding renderInfoUniformBinding;
        renderInfoUniformBinding.Binding = 1;
        renderInfoUniformBinding.Name = "RenderInfo";
        renderInfoUniformBinding.Size = sizeof(VB_UNIFORM_RENDERINFO);
        m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(renderInfoUniformBinding);
        m_TransformUniformBuffer->BindToShader(m_Shader);

        Nexus::Graphics::PipelineDescription pipelineDescription;
        pipelineDescription.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::Back;
        pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
        pipelineDescription.Shader = m_Shader;

        m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);

        Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
        m_SpriteMesh = factory.CreateCube();

        m_CommandList = m_GraphicsDevice->CreateCommandList();

        Nexus::Ref<Nexus::Audio::AudioBuffer> buffer = m_AudioDevice->CreateAudioBufferFromMP3File("Resources/Audio/Guitar_Music.mp3");
        Nexus::Ref<Nexus::Audio::AudioSource> source = m_AudioDevice->CreateAudioSource(buffer);
        m_AudioDevice->PlaySource(source);

        m_ShootSoundEffect = m_AudioDevice->CreateAudioBufferFromWavFile("Resources/Audio/Laser_Shoot.wav");
        m_ShootSoundSource = m_AudioDevice->CreateAudioSource(m_ShootSoundEffect);

        Demos::ClearScreenDemo clearScreenDemo(m_GraphicsDevice);*/

        auto &io = ImGui::GetIO();
        io.FontDefault = io.Fonts->AddFontFromFileTTF(
            "Resources/Fonts/Roboto/Roboto-Regular.ttf", 18);

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
        /* m_GraphicsDevice->SetFramebuffer(nullptr);
        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = GetWindowSize().X;
        vp.Height = GetWindowSize().Y;
        m_GraphicsDevice->SetViewport(vp);

        float aspectRatio = (float)GetWindowSize().X / (float)GetWindowSize().Y;

        m_CameraUniforms.View = m_Camera.GetView();
        m_CameraUniforms.Projection = m_Camera.GetProjection();
        m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

        m_RenderInfoUniforms.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));
        m_RenderInfoUniforms.Color = m_CubeColor;
        m_TransformUniformBuffer->SetData(&m_RenderInfoUniforms, sizeof(m_RenderInfoUniforms), 0);

        Nexus::Graphics::TextureBinding textureBinding;
        textureBinding.Slot = 0;
        textureBinding.Name = "texSampler";

        m_Shader->SetTexture(m_Texture, textureBinding);

        Nexus::Graphics::CommandListBeginInfo beginInfo{};
        beginInfo.ClearValue = {
            m_ClearColor.r,
            m_ClearColor.g,
            m_ClearColor.b,
            1.0f};

        m_CommandList->Begin(beginInfo);
        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->SetVertexBuffer(m_SpriteMesh.GetVertexBuffer());
        m_CommandList->SetIndexBuffer(m_SpriteMesh.GetIndexBuffer());
        m_CommandList->DrawIndexed(m_SpriteMesh.GetIndexBuffer()->GetIndexCount(), 0);
        m_CommandList->End();
        m_GraphicsDevice->SubmitCommandList(m_CommandList);

        m_Camera.Update(
            GetWindowSize().X,
            GetWindowSize().Y,
            time);

        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColor));
        ImGui::ColorEdit3("Cube Colour", glm::value_ptr(m_CubeColor));
        ImGui::End();

        Nexus::Input::GamepadSetLED(0, m_ClearColor.r * 255, m_ClearColor.g * 255, m_ClearColor.b * 255);

        if (Nexus::Input::IsKeyPressed(Nexus::KeyCode::Space))
        {
            m_AudioDevice->PlaySource(m_ShootSoundSource);
        } */

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
    /* Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
    Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;
    Nexus::Ref<Nexus::Graphics::UniformBuffer> m_TransformUniformBuffer;
    Nexus::Ref<Nexus::Graphics::Texture> m_Texture;
    Nexus::Ref<Nexus::Graphics::Framebuffer> m_Framebuffer;
    Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
    Nexus::Graphics::Mesh m_SpriteMesh;

    Nexus::FirstPersonCamera m_Camera;

    VB_UNIFORM_CAMERA_DEMO_3D m_CameraUniforms;
    VB_UNIFORM_TRANSFORM_DEMO_3D m_RenderInfoUniforms;

    glm::vec3 m_ClearColor{0.8f, 0.2f, 0.3f};
    glm::vec3 m_CubeColor{1.0f, 1.0f, 1.0f};

    Nexus::Ref<Nexus::Audio::AudioBuffer> m_ShootSoundEffect;
    Nexus::Ref<Nexus::Audio::AudioSource> m_ShootSoundSource;
    Nexus::Point<int> m_PreviousWindowSize; */

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