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

std::vector<float> vertices1 = {
    -0.5f, -0.5f, 0.0f, 0, 0,
    0.5f, -0.5f, 0.0f, 1, 0, 
    0.5f, 0.5f, 0.0f, 1, 1,
    -0.5f, 0.5f, 0.0f, 0, 1,
};

unsigned int indices1[] = {
	0, 1, 2,
	2, 3, 0
};

std::vector<float> vertices2 = {
    0.0f, 0.0f, 0.0f, 0, 0,
    1.0f, 0.0f, 0.0f, 1, 0,
    1.0f, 1.0f, 0.0f, 1, 1,
    0.0f, 1.0f, 0.0f, 0, 1,
};

unsigned int indices2[] = {
	0, 1, 2,
	2, 3, 0
};

#ifndef __EMSCRIPTEN__
    std::string vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;"
    "layout (location = 1) in vec2 aTexCoord;"
    "out vec2 TexCoord;"
    "uniform mat4 Transform;"
    "void main()"
    "{"
    "    gl_Position = Transform * vec4(aPos, 1.0);"
    "    TexCoord = aTexCoord;"
    "}";

    std::string fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;"
    "out vec4 Color2;"
    "in vec2 TexCoord;"
    "uniform sampler2D ourTexture;"
    "uniform vec4 TintColor;"
    "uniform mat4 Transform;"
    "void main()"
    "{"
    "    FragColor = texture(ourTexture, TexCoord) * TintColor;"
    "    Color2 = vec4(0, 1, 0, 1);"
    "}";
#else
    std::string vertexShaderSource = 
    "#version 300 es\n"
    "precision mediump float;"
    "precision highp int;"
    "layout (location = 0) in vec3 aPos;"
    "layout (location = 1) in vec2 aTexCoord;"
    "out vec2 TexCoord;"
    "uniform mat4 Transform;"
    "void main()"
    "{"
    "    gl_Position = Transform * vec4(aPos, 1.0);"
    "    TexCoord = aTexCoord;"
    "}";

    std::string fragmentShaderSource = 
    "#version 300 es\n"
    "precision mediump float;"
    "precision highp int;"
    "layout (location = 0) out vec4 FragColor;"
    "layout (location = 1) out vec4 Color2;"
    "in vec2 TexCoord;"
    "uniform sampler2D ourTexture;"
    "uniform vec4 TintColor;"
    "uniform mat4 Transform;"
    "void main()"
    "{"
    "    FragColor = texture(ourTexture, TexCoord) * TintColor;"
    "    Color2 = vec4(0, 1, 0, 1);"
    "}";
#endif

class Editor : public Nexus::Application
{
    public:
        Editor() : Application(Nexus::GraphicsAPI::OpenGL){}

        virtual void Load() override
        {
            ImGui::LoadIniSettingsFromDisk("Layout.ini");

            this->m_Renderer = Nexus::Renderer::Create(this->m_GraphicsDevice);

            this->m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Enabled);
            //this->m_Shader = this->m_GraphicsDevice->CreateShaderFromFile("Resources/Shaders/Basic.shader");
            //m_Shader = m_GraphicsDevice->CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);

            Nexus::BufferLayout layout = 
            {
                { Nexus::ShaderDataType::Float3, "aPos" },
                { Nexus::ShaderDataType::Float2, "aTexCoord"}
            };

            #ifndef __EMSCRIPTEN__
                m_Shader = m_GraphicsDevice->CreateShaderFromFile("Resources/Shaders/GLSL.shader", layout);
            #else
                m_Shader = m_GraphicsDevice->CreateShaderFromFile("Resources/Shaders/GLSLES.shader", layout);
            #endif

            this->m_VertexBuffer1 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices1);
            this->m_IndexBuffer1 = this->m_GraphicsDevice->CreateIndexBuffer(indices1, sizeof(indices1));

            /* this->m_VertexBuffer2 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices2);
            this->m_IndexBuffer2 = this->m_GraphicsDevice->CreateIndexBuffer(indices2, sizeof(indices2)); */

            m_Texture1 = this->m_GraphicsDevice->CreateTexture("Resources/Textures/brick.jpg");
            m_Texture2 = this->m_GraphicsDevice->CreateTexture("Resources/Textures/wall.jpg");
            
            Nexus::Point size = this->GetWindowSize();
            this->m_Camera = { size.Width, size.Height, {0, 0, 0} };

            NX_LOG("This is a log");
            NX_WARNING("This is a warning");
            NX_ERROR("This is an error");

            Nexus::FramebufferSpecification framebufferSpec;
            framebufferSpec.Width = size.Width;
            framebufferSpec.Height = size.Height;
            framebufferSpec.ColorAttachmentSpecification = { Nexus::TextureFormat::RGBA8, Nexus::TextureFormat::RGBA8 };
            framebufferSpec.DepthAttachmentSpecification = Nexus::DepthFormat::DEPTH24STENCIL8;
            m_Framebuffer = this->m_GraphicsDevice->CreateFramebuffer(framebufferSpec);

            auto& style = ImGui::GetStyle();
            style.ChildBorderSize = 0.0f;

            /* std::ifstream file("Resources/test.txt");
            std::string line;

            while (getline(file, line))
            {
                std::cout << line << std::endl;
            } */

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

            this->m_GraphicsDevice->SetContext();

            auto windowSize = this->GetWindowSize();
            auto framebufferSpec = this->m_Framebuffer->GetFramebufferSpecification();
            this->m_GraphicsDevice->Resize({framebufferSpec.Width, framebufferSpec.Height});

            this->m_Framebuffer->Bind();
            this->m_Renderer->Begin(glm::mat4(0), glm::vec4(0.07f, 0.13f, 0.17f, 1));

            this->m_Shader->Bind();
            this->m_Shader->SetShaderUniform4f("TintColor", glm::vec4(0.7f, 0.1f, 0.2f, 1));
            this->m_Shader->SetShaderUniform1i("ourTexture", 0);

            this->RenderQuad(m_Texture1, m_QuadPosition, m_QuadSize);           

            this->m_Renderer->End();
            this->m_Framebuffer->Unbind();

            Nexus::Point size = this->GetWindowSize();
            this->m_GraphicsDevice->Resize(size);
            this->m_Camera.Resize(size.Width, size.Height);
            this->m_GraphicsDevice->Clear(0, 0, 0, 1);
            this->BeginImGuiRender();
            RenderEditorUI();
            this->EndImGuiRender();

            this->m_GraphicsDevice->SwapBuffers();
        }

        virtual void OnResize(Nexus::Point size) override
        {
            Nexus::FramebufferSpecification spec = m_Framebuffer->GetFramebufferSpecification();
            spec.Width = size.Width;
            spec.Height = size.Height;
            m_Framebuffer->SetFramebufferSpecification(spec);
        }

        virtual bool OnClose() override
        {
            ImGui::SaveIniSettingsToDisk("Layout.ini");
            return true;
        }

        void RenderQuad(Nexus::Ref<Nexus::Texture> texture, const glm::vec3& position, const glm::vec3& scale)
        {
            texture->Bind();

            glm::mat4 mvp = this->m_Camera.GetProjection() * m_Camera.GetWorld() * glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0f), position);
            this->m_Shader->SetShaderUniformMat4("Transform", mvp);

            this->m_GraphicsDevice->DrawIndexed(this->m_VertexBuffer1, this->m_IndexBuffer1);
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
                //std::wstring path(p);
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

            ImGui::SetNextWindowPos({
                (float)GetWindowPosition().Width,
                (float)GetWindowPosition().Height
            }, ImGuiCond_Always);
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
        Nexus::Ref<Nexus::VertexBuffer> m_VertexBuffer1;
        Nexus::Ref<Nexus::IndexBuffer> m_IndexBuffer1;

        Nexus::Ref<Nexus::Texture> m_Texture2;
        Nexus::Ref<Nexus::VertexBuffer> m_VertexBuffer2;
        Nexus::Ref<Nexus::IndexBuffer> m_IndexBuffer2;

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