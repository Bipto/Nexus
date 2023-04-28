#include "NexusEngine.h"

#ifndef __EMSCRIPTEN__
#include "tinyfiledialogs.h"
#include "more_dialogs/tinyfd_moredialogs.h"
#endif

#include "UI/Panel.h"
#include "UI/SceneHierarchyPanel.h"
#include "UI/SettingsPanel.h"
#include "UI/InspectorPanel.h"
#include "UI/LogPanel.h"
#include "UI/ViewportPanel.h"
#include "UI/NewProjectPanel.h"
#include "UI/AboutPanel.h"
#include "UI/ProjectHierarchyPanel.h"
#include "UI/NewScenePanel.h"

std::vector<float> vertices = 
{
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  //bottom left
    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  //top left
     0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  //bottom right
     0.5f,  0.5f, 0.0f, 0.0f, 1.0f   //top right
};

std::vector<unsigned int> indices = 
{
    0, 1, 2,
    1, 3, 2
};

class Editor : public Nexus::Application
{
    public:
        Editor(const Nexus::ApplicationSpecification& spec) : Application(spec){}

        virtual void Load() override
        {
            ImGui::LoadIniSettingsFromDisk("Layout.ini");

            this->m_Renderer = Nexus::Renderer::Create(this->m_GraphicsDevice);
            this->m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Enabled);

            Nexus::BufferLayout layout = 
            {
                { Nexus::ShaderDataType::Float3, "TEXCOORD", 0 },
                { Nexus::ShaderDataType::Float2, "TEXCOORD", 1 }
            };

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("Resources/Shaders/shader.glsl", layout);

            this->m_VertexBuffer =  this->m_GraphicsDevice->CreateVertexBuffer(vertices);
            this->m_IndexBuffer = this->m_GraphicsDevice->CreateIndexBuffer(indices);

            m_Texture1 = this->m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");
            m_Texture2 = this->m_GraphicsDevice->CreateTexture("Resources/Textures/wall.jpg");
            
            Nexus::Point size = this->GetWindowSize();
            this->m_Camera = { size.Width, size.Height, {0, 0, 0} };

            Nexus::FramebufferSpecification framebufferSpec;
            framebufferSpec.Width = size.Width;
            framebufferSpec.Height = size.Height;
            framebufferSpec.ColorAttachmentSpecification = { Nexus::TextureFormat::RGBA8, Nexus::TextureFormat::RGBA8 };
            framebufferSpec.DepthAttachmentSpecification = Nexus::DepthFormat::DEPTH24STENCIL8;
            m_Framebuffer = this->m_GraphicsDevice->CreateFramebuffer(framebufferSpec);
            
            auto& style = ImGui::GetStyle();
            style.ChildBorderSize = 0.0f;

            auto& io = ImGui::GetIO();
            io.FontDefault = io.Fonts->AddFontFromFileTTF(
                "Resources/Fonts/Roboto/Roboto-Regular.ttf", 18
            );

            CreatePanels();
        }

        void CreatePanels()
        {
            SceneHierarchyPanel* sceneHierarchyPanel = new SceneHierarchyPanel();
            std::function<void(int)> f = std::bind(&Editor::OnEntitySelected, this, std::placeholders::_1); 
            Delegate<int> d("OnEntitySelected", f);
            sceneHierarchyPanel->BindEntitySelectedFunction(d);

            m_Panels["SceneHierarchy"] = sceneHierarchyPanel;
            m_Panels["Settings"] = new SettingsPanel(&m_MovementSpeed,
                                                    &m_QuadPosition,
                                                    &m_QuadSize);
            m_Panels["InspectorPanel"] = new InspectorPanel();
            m_Panels["LogPanel"] = new LogPanel();
            m_Panels["ViewportPanel"] = new ViewportPanel(m_Framebuffer);
            m_Panels["ProjectHierarchy"] = new ProjectHierarchyPanel();

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

            ApplyDarkTheme();
        }

        void OnEntitySelected(int entityID)
        {
            for (auto& panel : m_Panels)
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
            for (auto panel : m_Panels)
            {
                if (panel.second->IsEnabled())
                    panel.second->OnUpdate();
            }

            //movement
            {
                auto pos = m_Camera.GetPosition();

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyUp))
                    pos.y -= m_MovementSpeed * time.GetSeconds();

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyDown))
                    pos.y += m_MovementSpeed * time.GetSeconds();

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyLeft))
                    pos.x -= m_MovementSpeed * time.GetSeconds();

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyRight))
                    pos.x += m_MovementSpeed * time.GetSeconds();

                m_Camera.SetPosition(pos);
            }                            

            //to framebuffer
            {
                m_GraphicsDevice->SetFramebuffer(m_Framebuffer);
                Nexus::Viewport vp;
                vp.X = 0;
                vp.Y = 0;
                vp.Width = m_Framebuffer->GetFramebufferSpecification().Width;
                vp.Height = m_Framebuffer->GetFramebufferSpecification().Height;
                m_GraphicsDevice->SetViewport(vp);
                m_GraphicsDevice->Clear(0.0f, 1.0f, 0.0f, 1.0f);
                m_GraphicsDevice->SetShader(m_Shader);
                m_GraphicsDevice->SetVertexBuffer(m_VertexBuffer);
                m_GraphicsDevice->SetIndexBuffer(m_IndexBuffer);
                m_GraphicsDevice->DrawIndexed(Nexus::PrimitiveType::Triangle,
                    m_IndexBuffer->GetIndexCount(),
                    0);
            }

            //to swapchain
            {
                m_GraphicsDevice->SetFramebuffer(nullptr);
                //BeginImGuiRender();
                RenderEditorUI();
                //EndImGuiRender();
            }
        }

        virtual void OnResize(Nexus::Point size) override
        {
            m_GraphicsDevice->Resize(size);
        }

        virtual bool OnClose() override
        {
            ImGui::SaveIniSettingsToDisk("Layout.ini");
            return true;
        }

        void RenderQuad(Nexus::Ref<Nexus::Texture> texture, const glm::vec3& position, const glm::vec3& scale)
        {
            /* texture->Bind();

            glm::mat4 mvp = this->m_Camera.GetProjection() * m_Camera.GetWorld() * glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0f), position);
            this->m_Shader->SetShaderUniformMat4("Transform", mvp);

            this->m_GraphicsDevice->DrawIndexed(this->m_VertexBuffer, this->m_IndexBuffer, this->m_Shader); */
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
            const char* filterPatterns[2] = {"*.proj", "*.text"};

            auto p = tinyfd_openFileDialog(
                "Select file",
                "",
                2,
                filterPatterns,
                "text files (*.txt|*.text)",
                0
            );

            if (p)
            {
                m_Project = Nexus::Project::Deserialize({p});
                LoadProjectIntoEditor();
            }

            #endif
        }

        void LoadProjectIntoEditor()
        {
            for (auto& panel : m_Panels)
                panel.second->LoadProject(m_Project);
        }

        void SetSelectedEntity(int entityID)
        {
            for (auto& panel : m_Panels)
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
            auto& colors = ImGui::GetStyle().Colors;
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
        Nexus::Renderer* m_Renderer;
        Nexus::Ref<Nexus::Shader> m_Shader;

        Nexus::Ref<Nexus::Texture> m_Texture1;
        Nexus::Ref<Nexus::Texture> m_Texture2;
        Nexus::Ref<Nexus::VertexBuffer> m_VertexBuffer;
        Nexus::Ref<Nexus::IndexBuffer> m_IndexBuffer;

        Nexus::OrthographicCamera m_Camera;
        Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;
        Nexus::Ref<Nexus::Project> m_Project;

        bool m_WindowOpen = true;

        Nexus::EventHandler<const std::string&> m_EventHandler;

        float m_MovementSpeed = 1.0f;
        glm::vec3 m_QuadPosition = {0, 0, 0};
        glm::vec3 m_QuadSize = {500, 500, 500};

        std::unordered_map<std::string, Panel*> m_Panels;
};