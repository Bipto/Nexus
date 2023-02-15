#include "NexusEngine.h"

#include <iostream>
#include <sstream>
#include <functional>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Components/Camera.h"
#include "Core/Graphics/TextureFormat.h"
#include "Core/Graphics/DepthFormat.h"

std::vector<Vertex> vertices1 = {
    {{-0.5f, -0.5f, 0.0f}, {0, 0}},
    {{0.5f, -0.5f, 0.0f}, {1, 0}},
    {{0.5f, 0.5f, 0.0f}, {1, 1}},
    {{-0.5f, 0.5f, 0.0f}, {0, 1}},
};

unsigned int indices1[] = {
	0, 1, 2,
	2, 3, 0
};

std::vector<Vertex> vertices2 = {
    {{0.0f, 0.0f, 0.0f}, {0, 0}},
    {{1.0f, 0.0f, 0.0f}, {1, 0}},
    {{1.0f, 1.0f, 0.0f}, {1, 1}},
    {{0.0f, 1.0f, 0.0f}, {0, 1}},
};

unsigned int indices2[] = {
	0, 1, 2,
	2, 3, 0
};

const char * vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 Transform;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = Transform * vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
	"}\n";

const char * fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
    "out vec4 Color2;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "uniform vec4 TintColor;\n"
    "uniform mat4 Transform;\n"
	"void main()\n"
	"{\n"
	"	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "   FragColor = texture(ourTexture, TexCoord) * TintColor;\n"
    "   Color2 = vec4(0, 1, 0, 1);\n"
	"}\n";

class Editor : public Nexus::Application
{
    public:
        Editor() : Application(Nexus::GraphicsAPI::OpenGL){}

        virtual void Load() override
        {
            this->m_Renderer = Nexus::Renderer::Create(this->m_GraphicsDevice);

            this->m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Enabled);
            this->m_Shader = this->m_GraphicsDevice->CreateShader(vertexShaderSource, fragmentShaderSource);

            this->m_VertexBuffer1 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices1);
            this->m_IndexBuffer1 = this->m_GraphicsDevice->CreateIndexBuffer(indices1, sizeof(indices1));

            /* this->m_VertexBuffer2 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices2);
            this->m_IndexBuffer2 = this->m_GraphicsDevice->CreateIndexBuffer(indices2, sizeof(indices2)); */

            m_Texture1 = this->m_GraphicsDevice->CreateTexture("brick.jpg");
            m_Texture2 = this->m_GraphicsDevice->CreateTexture("wall.jpg");
            
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

            m_PreviousSize = size;

            //static method
            //m_EventHandler.Bind(PrintText);


            std::function<void(const std::string& text)> f = std::bind(&Editor::PrintText, this, std::placeholders::_1);
            Delegate<const std::string&> d("PrintText", f);
            
            m_EventHandler.Bind(d);

            //m_EventHandler.Unbind(d);
            m_EventHandler.Invoke("Event firing test!");
            m_EventHandler.Invoke("Event numero dos");
        }

        virtual void Update() override
        {
            //movement
            {
                auto pos = m_Camera.GetPosition();

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyUp))
                    pos.y -= 1;

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyDown))
                    pos.y += 1;

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyLeft))
                    pos.x -= 1;

                if (NX_IS_KEY_PRESSED(Nexus::KeyCode::KeyRight))
                    pos.x += 1;

                m_Camera.SetPosition(pos);
            }
                            

            this->m_GraphicsDevice->SetContext();

            auto windowSize = this->GetWindowSize();
            /* if (m_PreviousSize.Width != windowSize.Width || m_PreviousSize.Height != windowSize.Height)
            {
                Nexus::FramebufferSpecification spec = m_Framebuffer->GetFramebufferSpecification();
                spec.Width = windowSize.Width;
                spec.Height = windowSize.Height;
                m_Framebuffer->SetFramebufferSpecification(spec);

                std::stringstream ss;
                ss << "Resizing - Width: " << spec.Width << " Height: " << spec.Height; 
                NX_LOG(ss.str());
            } */

            auto framebufferSpec = this->m_Framebuffer->GetFramebufferSpecification();
            this->m_GraphicsDevice->Resize({framebufferSpec.Width, framebufferSpec.Height});

            this->m_Framebuffer->Bind();
            this->m_Renderer->Begin(glm::mat4(0), glm::vec4(0.07f, 0.13f, 0.17f, 1));

            this->m_Shader->Bind();
            this->m_Shader->SetShaderUniform4f("TintColor", glm::vec4(0.7f, 0.1f, 0.2f, 1));
            this->m_Shader->SetShaderUniform1i("ourTexture", 0);

            this->RenderQuad(m_Texture1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(500.0f, 500.0f, 500.0f));           

            this->m_Renderer->End();
            this->m_Framebuffer->Unbind();

            Nexus::Point size = this->GetWindowSize();
            this->m_GraphicsDevice->Resize(size);
            this->m_Camera.Resize(size.Width, size.Height);
            this->m_GraphicsDevice->Clear(0, 0, 0, 1);
            this->BeginImGuiRender();
            this->m_PreviousSize = size;

            ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);


            flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin("Dockspace Demo", nullptr, flags))
            {
                ImGui::DockSpace(ImGui::GetID("Dockspace"));
            
                if (ImGui::BeginMainMenuBar())
                {
                    if (ImGui::BeginMenu("File", true))
                    {
                        if (ImGui::MenuItem("Quit"))
                        {
                            this->Close();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMainMenuBar();
                }            

                if (m_WindowOpen)
                {
                    ImGui::Begin("My Window", &m_WindowOpen);
                    auto availSize = ImGui::GetContentRegionAvail();
                    if (m_Framebuffer->HasColorTexture())
                        ImGui::Image((void*)m_Framebuffer->GetColorAttachment(0), availSize);
                    ImGui::End();
                }

                ImGui::End();
            }

            ImGui::PopStyleVar(2);

            this->EndImGuiRender();

            this->m_GraphicsDevice->SwapBuffers();
        }

        void RenderQuad(Nexus::Texture* texture, const glm::vec3& position, const glm::vec3& scale)
        {
            texture->Bind();
            this->m_VertexBuffer1->Bind();
            this->m_IndexBuffer1->Bind();

            glm::mat4 mvp = this->m_Camera.GetProjection() * m_Camera.GetWorld() * glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0f), position);
            this->m_Shader->SetShaderUniformMat4("Transform", mvp);

            this->m_GraphicsDevice->DrawIndexed(6);
        }

        void Unload()
        {
            
        }

        void PrintText(const std::string& text)
        {
            std::cout << text << std::endl;
        }

    private:
        Nexus::Renderer* m_Renderer;

        Nexus::Point m_PreviousSize;
        Nexus::Shader* m_Shader;

        Nexus::Texture* m_Texture1;
        Nexus::VertexBuffer* m_VertexBuffer1;
        Nexus::IndexBuffer* m_IndexBuffer1;

        Nexus::Texture* m_Texture2;
        Nexus::VertexBuffer* m_VertexBuffer2;
        Nexus::IndexBuffer* m_IndexBuffer2;

        Nexus::OrthographicCamera m_Camera;
        Nexus::Framebuffer* m_Framebuffer;

        bool m_WindowOpen = true;

        Nexus::EventHandler<const std::string&> m_EventHandler;
};

int main(int argc, char** argv)
{
    Nexus::Init();

    Editor* app = new Editor();
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
} 