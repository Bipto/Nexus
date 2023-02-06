#include "NexusEngine.h"

#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Components/Camera.h"

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
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "uniform vec4 TintColor;\n"
    "uniform mat4 Transform;\n"
	"void main()\n"
	"{\n"
	"	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "   FragColor = texture(ourTexture, TexCoord) * TintColor;\n"
	"}\n";

class Editor : public Nexus::Application
{
    public:
        Editor() : Application(Nexus::GraphicsAPI::OpenGL){}

        void Load() override
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
            
            Nexus::Size size = this->GetWindowSize();
            this->m_Camera = { size.Width, size.Height };

            NX_LOG("This is a log");
            NX_WARNING("This is a warning");
            NX_ERROR("This is an error");

            Nexus::FramebufferSpecification framebufferSpec;
            framebufferSpec.Width = 1920;
            framebufferSpec.Height = 1080;
            m_Framebuffer = this->m_GraphicsDevice->CreateFramebuffer(framebufferSpec);
        }

        void Update()
        {
            this->m_GraphicsDevice->SetContext();

            auto windowSize = this->GetWindowSize();
            if (m_PreviousSize.Width != windowSize.Width || m_PreviousSize.Height != windowSize.Height)
            {
                Nexus::FramebufferSpecification spec;
                spec.Width = windowSize.Width;
                spec.Height = windowSize.Height;
                m_Framebuffer->SetFramebufferSpecification(spec);

                std::stringstream ss;
                ss << "Resizing - Width: " << spec.Width << " Height: " << spec.Height; 
                NX_LOG(ss.str());
            }

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

            Nexus::Size size = this->GetWindowSize();
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

                if (m_WindowOpen)
                {
                    ImGui::Begin("My Window", &m_WindowOpen);
                    auto availSize = ImGui::GetContentRegionAvail();
                    ImGui::Image((void*)m_Framebuffer->GetColorAttachment(), availSize);
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

            glm::mat4 mvp = this->m_Camera.GetProjection() * glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0f), position);
            this->m_Shader->SetShaderUniformMat4("Transform", mvp);

            this->m_GraphicsDevice->DrawIndexed(6);
        }

        void Unload()
        {
            
        }
    private:
        Nexus::Renderer* m_Renderer;

        Nexus::Size m_PreviousSize;
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