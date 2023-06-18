#include "NexusEngine.h"

#include "UI/Panel.h"
#include "UI/SceneHierarchyPanel.h"
#include "UI/InspectorPanel.h"
#include "UI/LogPanel.h"
#include "UI/ViewportPanel.h"
#include "UI/NewProjectPanel.h"
#include "UI/AboutPanel.h"
#include "UI/ProjectHierarchyPanel.h"
#include "UI/NewScenePanel.h"
#include "UI/ShaderViewerPanel.h"

#include "Core/Utils/FileDialogs.h"

#include "Core/Graphics/MeshFactory.h"

std::vector<Vertex> vertices =
    {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}}, // bottom left
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},  // top left
        {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},  // bottom right
        {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}}    // top right
};

std::vector<unsigned int> indices =
    {
        0, 1, 2,
        1, 3, 2};

struct alignas(16) VB_UNIFORM_RENDERINFO
{
    glm::mat4 Translation;
    glm::vec3 Color;
};

struct alignas(16) VB_UNIFORM_CAMERA
{
    glm::mat4 View;
    glm::mat4 Projection;
};

class Editor : public Nexus::Application
{
public:
    Editor(const Nexus::ApplicationSpecification &spec) : Application(spec) {}

    virtual void Load() override
    {
        ImGui::LoadIniSettingsFromDisk("Layout.ini");

        this->m_Renderer = Nexus::Renderer::Create(this->m_GraphicsDevice);
        this->m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Enabled);

        this->m_VertexBuffer = this->m_GraphicsDevice->CreateVertexBuffer(vertices);
        this->m_IndexBuffer = this->m_GraphicsDevice->CreateIndexBuffer(indices);
        this->m_Texture = m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");

        Nexus::UniformResourceBinding renderInfoBinding;
        renderInfoBinding.Binding = 0;
        renderInfoBinding.Name = "RenderInfo";
        renderInfoBinding.Size = sizeof(VB_UNIFORM_RENDERINFO);

        m_RenderInfoUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(renderInfoBinding);

        Nexus::UniformResourceBinding cameraUniformBinding;
        cameraUniformBinding.Binding = 1;
        cameraUniformBinding.Name = "Camera";
        cameraUniformBinding.Size = sizeof(VB_UNIFORM_CAMERA);

        m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBinding);

        Nexus::VertexBufferLayout vertexBufferLayout =
            {
                {Nexus::ShaderDataType::Float3, "TEXCOORD", 0},
                {Nexus::ShaderDataType::Float2, "TEXCOORD", 1}};

        m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/shader.glsl", vertexBufferLayout);

        m_TextureBinding.Slot = 0;
        m_TextureBinding.Name = "texSampler";

        Nexus::Point size = this->GetWindowSize();
        Nexus::FramebufferSpecification framebufferSpec;
        framebufferSpec.Width = 500;
        framebufferSpec.Height = 500;
        framebufferSpec.ColorAttachmentSpecification = {Nexus::TextureFormat::RGBA8, Nexus::TextureFormat::RGBA8};
        framebufferSpec.DepthAttachmentSpecification = Nexus::DepthFormat::DEPTH24STENCIL8;
        m_Framebuffer = this->m_GraphicsDevice->CreateFramebuffer(framebufferSpec);

        Nexus::PipelineDescription pipelineDescription;
        pipelineDescription.RasterizerStateDescription.CullMode = Nexus::CullMode::None;
        pipelineDescription.RasterizerStateDescription.FrontFace = Nexus::FrontFace::CounterClockwise;
        pipelineDescription.Shader = m_Shader;
        m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);

        m_CommandList = m_GraphicsDevice->CreateCommandList(m_Pipeline);

        auto &style = ImGui::GetStyle();
        style.ChildBorderSize = 0.0f;

        auto &io = ImGui::GetIO();
        io.FontDefault = io.Fonts->AddFontFromFileTTF(
            "Resources/Fonts/Roboto/Roboto-Regular.ttf", 18);

        CreatePanels();

        Nexus::MeshFactory factory = Nexus::MeshFactory(m_GraphicsDevice);
        m_SpriteMesh = factory.CreateSprite();

        m_Camera.SetPosition({0.0f, 0.0f, -5.0f});
    }

    void CreatePanels()
    {
        SceneHierarchyPanel *sceneHierarchyPanel = new SceneHierarchyPanel();
        std::function<void(int)> f = std::bind(&Editor::OnEntitySelected, this, std::placeholders::_1);
        Delegate<int> d("OnEntitySelected", f);
        sceneHierarchyPanel->BindEntitySelectedFunction(d);

        m_Panels["SceneHierarchy"] = sceneHierarchyPanel;

        m_InspectorPanel = new InspectorPanel();
        m_Panels["InspectorPanel"] = m_InspectorPanel;
        m_Panels["LogPanel"] = new LogPanel();
        m_Panels["ProjectHierarchy"] = new ProjectHierarchyPanel();
        m_ViewportPanel = new ViewportPanel(m_Framebuffer, m_GraphicsDevice);
        m_Panels["ViewportPanel"] = m_ViewportPanel;

        auto newScenePanel = new NewScenePanel();
        newScenePanel->Disable();
        m_Panels["NewScenePanel"] = newScenePanel;

        auto newProjectPanel = new NewProjectPanel();
        auto function = std::bind(&Editor::OnProjectCreated, this, std::placeholders::_1);
        Delegate<Nexus::Ref<Nexus::Project>> onEntitySelectedDelegate("OnProjectCreated", function);
        newProjectPanel->Subscribe(onEntitySelectedDelegate);
        newProjectPanel->Disable();
        m_Panels["NewProjectPanel"] = newProjectPanel;

        auto aboutPanel = new AboutPanel(m_GraphicsDevice);
        aboutPanel->Disable();
        m_Panels["AboutPanel"] = aboutPanel;

        m_Panels["ShaderViewerPanel"] = new ShaderViewerPanel(m_Shader);

        ApplyDarkTheme();
    }

    void OnEntitySelected(int entityID)
    {
        for (auto &panel : m_Panels)
            panel.second->SetActiveEntity(entityID);
    }

    void OnProjectCreated(Nexus::Ref<Nexus::Project> project)
    {
        m_Project = project;
        LoadProjectIntoEditor();
    }

    virtual void Update(Nexus::Time time) override
    {
    }

    virtual void Render(Nexus::Time time) override
    {
        if (m_ViewportPanel->FramebufferRequiresResize())
        {
            auto spec = m_Framebuffer->GetFramebufferSpecification();
            spec.Width = m_ViewportPanel->GetWindowSize().x;
            spec.Height = m_ViewportPanel->GetWindowSize().y;

            if (spec.Width > 0 && spec.Height > 0)
            {
                m_Framebuffer->SetFramebufferSpecification(spec);
            }
        }

        m_GraphicsDevice->SetPipeline(m_Pipeline);

        // to framebuffer
        {
            m_GraphicsDevice->SetFramebuffer(m_Framebuffer);
            Nexus::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = m_Framebuffer->GetFramebufferSpecification().Width;
            vp.Height = m_Framebuffer->GetFramebufferSpecification().Height;
            m_GraphicsDevice->SetViewport(vp);

            m_GraphicsDevice->SetFramebuffer(m_Framebuffer);

            // if project has been loaded, render entities
            if (m_Project)
            {
                auto activeScene = m_Project->GetActiveScene();
                if (activeScene)
                {
                    auto clearColor = activeScene->GetClearColor();
                    Nexus::CommandListBeginInfo beginInfo{};
                    beginInfo.ClearValue = {
                        clearColor.r,
                        clearColor.g,
                        clearColor.b,
                        clearColor.a};

                    m_CommandList->Begin(beginInfo);

                    for (auto &entity : activeScene->GetEntities())
                    {
                        if (entity.HasComponent<Nexus::SpriteRendererComponent>() && entity.HasComponent<Nexus::TransformComponent>())
                        {
                            auto transformComponent = entity.GetComponent<Nexus::TransformComponent *>();
                            auto spriteRendererComponent = entity.GetComponent<Nexus::SpriteRendererComponent *>();

                            if (spriteRendererComponent->GetTexture())
                            {
                                RenderQuad(
                                    spriteRendererComponent->GetTexture(),
                                    transformComponent->GetTranslation(),
                                    transformComponent->GetScale(),
                                    spriteRendererComponent->GetColor(),
                                    m_CommandList);
                            }
                        }
                    }

                    m_CommandList->End();
                    m_GraphicsDevice->SubmitCommandList(m_CommandList);
                }
            }

            // otherwise render empty screen
            else
            {
                Nexus::CommandListBeginInfo beginInfo{};
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

        // to swapchain
        {
            m_GraphicsDevice->SetFramebuffer(nullptr);
            RenderEditorUI();
        }

        // movement
        {
            m_Camera.Update(
                GetWindowSize().X,
                GetWindowSize().Y,
                time);
            m_Camera.Resize(m_ViewportPanel->GetWindowSize().x, m_ViewportPanel->GetWindowSize().y);

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);
        }

        if (Nexus::Input::WasGamepadKeyPressed(0, Nexus::GamepadButton::A))
        {
            NX_LOG("A button pressed");
        }

        if (Nexus::Input::WasGamepadKeyReleased(0, Nexus::GamepadButton::A))
        {
            NX_LOG("A button released");
        }
    }

    virtual void OnResize(Nexus::Point<int> size) override
    {
        m_GraphicsDevice->Resize(size);
    }

    virtual bool OnClose() override
    {
        ImGui::SaveIniSettingsToDisk("Layout.ini");
        return true;
    }

    void RenderQuad(Nexus::Ref<Nexus::Texture> texture, const glm::vec3 &position, const glm::vec3 &scale, const glm::vec3 &color, Nexus::Ref<Nexus::CommandList> commandList)
    {
        m_RenderInfoUniforms.Translation = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
        m_RenderInfoUniforms.Color = color;

        m_CommandList->UpdateUniformBuffer(
            m_RenderInfoUniformBuffer,
            &m_RenderInfoUniforms,
            sizeof(m_RenderInfoUniforms),
            0);

        m_CommandList->UpdateTexture(
            texture,
            m_Shader,
            m_TextureBinding);

        m_CommandList->SetVertexBuffer(m_SpriteMesh.GetVertexBuffer());
        m_CommandList->SetIndexBuffer(m_SpriteMesh.GetIndexBuffer());

        m_CommandList->DrawIndexed(
            m_SpriteMesh.GetIndexBuffer()->GetIndexCount(),
            0);
    }

    virtual void Unload() override
    {
    }

    void CreateNewProject()
    {
        m_Panels["NewProjectPanel"]->Enable();
    }

    void OpenProject()
    {
#ifndef __EMSCRIPTEN__

        std::vector<const char *> filters = {"*.proj"};
        auto p = Nexus::FileDialogs::OpenFile(filters);

        if (p)
        {
            m_Project = Nexus::Project::Deserialize({p});
            LoadProjectIntoEditor();
        }

#endif
    }

    void LoadProjectIntoEditor()
    {
        for (auto &panel : m_Panels)
            panel.second->LoadProject(m_Project);

        auto activeScene = m_Project->GetActiveScene();
    }

    void SetSelectedEntity(int entityID)
    {
        for (auto &panel : m_Panels)
            panel.second->SetActiveEntity(entityID);
    }

    void RenderMainMenubar()
    {
        ImGui::BeginMenuBar();

        if (ImGui::BeginMenu("File", true))
        {
            if (ImGui::BeginMenu("New"))
            {
                if (ImGui::MenuItem("New Project"))
                    m_Panels["NewProjectPanel"]->Enable();

                if (ImGui::MenuItem("New Scene"))
                    m_Panels["NewScenePanel"]->Enable();

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save"))
            {
                if (m_Project)
                {
                    auto directory = m_Project->GetProjectDirectory();
                    m_Project->Serialize(directory);
                }
            }

            if (ImGui::MenuItem("Open"))
            {
                OpenProject();
            }

            if (ImGui::MenuItem("Quit"))
            {
                this->Close();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                m_Panels["AboutPanel"]->Enable();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    void RenderEditorUI()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_MenuBar;

        if (ImGui::Begin("Dockspace", nullptr, flags))
        {
            ImGui::PopStyleVar(3);
            ImGui::DockSpace(ImGui::GetID("Dockspace"));

            for (auto panel : m_Panels)
            {
                if (panel.second->IsEnabled())
                    panel.second->OnRender();
            }

            RenderMainMenubar();
            ImGui::End();
        }
    }

    void ApplyDarkTheme()
    {
        auto &colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = {0.05f, 0.055f, 0.06f, 1.0f};

        colors[ImGuiCol_Header] = {0.15f, 0.15f, 0.16f, 1.0f};
        colors[ImGuiCol_HeaderHovered] = {0.3f, 0.3f, 0.31f, 1.0f};
        colors[ImGuiCol_HeaderActive] = {0.3f, 0.3f, 0.31f, 1.0f};

        colors[ImGuiCol_Button] = {0.15f, 0.15f, 0.15f, 1.0f};
        colors[ImGuiCol_ButtonHovered] = {0.25f, 0.25f, 0.25f, 1.0f};
        colors[ImGuiCol_ButtonActive] = {0.1f, 0.1f, 0.1f, 1.0f};

        colors[ImGuiCol_FrameBg] = {0.3f, 0.3f, 0.31f, 1.0f};
        colors[ImGuiCol_FrameBgHovered] = {0.4f, 0.4f, 0.41f, 1.0f};
        colors[ImGuiCol_FrameBgActive] = {0.3f, 0.3f, 0.31f, 1.0f};

        colors[ImGuiCol_Tab] = {0.15f, 0.15f, 0.15f, 1.0f};
        colors[ImGuiCol_TabHovered] = {0.3f, 0.3f, 0.31f, 1.0f};
        colors[ImGuiCol_TabActive] = {0.15f, 0.15f, 0.16f, 1.0f};
        colors[ImGuiCol_TabUnfocused] = {0.15f, 0.15f, 0.16f, 1.0f};
        colors[ImGuiCol_TabUnfocusedActive] = {0.15f, 0.15f, 0.16f, 1.0f};

        colors[ImGuiCol_MenuBarBg] = {0.08f, 0.085f, 0.09f, 1.0f};
        colors[ImGuiCol_TitleBg] = {0.05f, 0.055f, 0.05f, 1.0f};
        colors[ImGuiCol_TitleBgActive] = {0.08f, 0.085f, 0.09f, 1.0f};
        colors[ImGuiCol_TitleBgCollapsed] = {0.05f, 0.055f, 0.06f, 1.0f};

        colors[ImGuiCol_CheckMark] = {0.55f, 0.55f, 0.55f, 1.0f};
        colors[ImGuiCol_Text] = {0.85f, 0.85f, 0.85f, 1.0f};
        colors[ImGuiCol_SliderGrab] = {0.55f, 0.55f, 0.55f, 1.0f};
        colors[ImGuiCol_SliderGrabActive] = {0.55f, 0.55f, 0.55f, 1.0f};
    }

private:
    Nexus::Renderer *m_Renderer;
    Nexus::Ref<Nexus::Shader> m_Shader;

    Nexus::Ref<Nexus::VertexBuffer> m_VertexBuffer;
    Nexus::Ref<Nexus::IndexBuffer> m_IndexBuffer;
    Nexus::Ref<Nexus::UniformBuffer> m_RenderInfoUniformBuffer;
    Nexus::Ref<Nexus::UniformBuffer> m_CameraUniformBuffer;
    Nexus::Ref<Nexus::Texture> m_Texture;
    VB_UNIFORM_RENDERINFO m_RenderInfoUniforms;
    VB_UNIFORM_CAMERA m_CameraUniforms;
    Nexus::Ref<Nexus::Pipeline> m_Pipeline;
    Nexus::Ref<Nexus::CommandList> m_CommandList;
    Nexus::TextureBinding m_TextureBinding;

    Nexus::FirstPersonCamera m_Camera{};
    Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;
    Nexus::Ref<Nexus::Project> m_Project;

    bool m_WindowOpen = true;

    Nexus::EventHandler<const std::string &> m_EventHandler;

    std::unordered_map<std::string, Panel *> m_Panels;
    ViewportPanel *m_ViewportPanel;
    InspectorPanel *m_InspectorPanel;

    Nexus::Mesh m_SpriteMesh;

    float m_MovementSpeed = 5.0f;
};