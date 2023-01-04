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

            this->m_GraphicsDevice->GetSwapchain()->SetVSyncState(Nexus::VSyncState::Enabled);
            this->m_Shader = this->m_GraphicsDevice->CreateShader(vertexShaderSource, fragmentShaderSource);

            this->m_VertexBuffer1 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices1);
            this->m_IndexBuffer1 = this->m_GraphicsDevice->CreateIndexBuffer(indices1, sizeof(indices1));

            /* this->m_VertexBuffer2 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices2);
            this->m_IndexBuffer2 = this->m_GraphicsDevice->CreateIndexBuffer(indices2, sizeof(indices2)); */

            m_Texture1 = this->m_GraphicsDevice->CreateTexture("brick.jpg");
            m_Texture2 = this->m_GraphicsDevice->CreateTexture("wall.jpg");

            this->m_GraphicsDevice->GetResourceFactory().Print();

            NX_LOG("This is a log");
            NX_WARNING("This is a warning");
            NX_ERROR("This is an error");
        }

        void Update()
        {
            this->m_GraphicsDevice->SetContext();
            Nexus::Size size = this->GetWindowSize();

            if (size.Height != this->m_PreviousSize.Height || size.Width != this->m_PreviousSize.Width)
            {
                this->m_GraphicsDevice->GetSwapchain()->Resize(size);
            }

            this->m_Renderer->Begin(glm::mat4(0), glm::vec4(0.07f, 0.13f, 0.17f, 1));

            this->m_Shader->Bind();

            this->m_Shader->SetShaderUniformMat4("Transform", this->m_MVP);

            this->m_Texture1->Bind();
            this->m_Shader->SetShaderUniform4f("TintColor", glm::vec4(0.7f, 0.1f, 0.2f, 1));
            this->m_Shader->SetShaderUniform1i("ourTexture", 0);
            this->m_VertexBuffer1->Bind();
            this->m_IndexBuffer1->Bind();            
            this->m_GraphicsDevice->DrawIndexed(6);

            /* this->m_Texture2->Bind();
            this->m_Shader->SetShaderUniform4f("TintColor", glm::vec4(0.15f, 0.8f, 0.2f, 1));
            this->m_Shader->SetShaderUniform1i("ourTexture", 0);
            this->m_VertexBuffer2->Bind();
            this->m_IndexBuffer2->Bind();            
            this->m_GraphicsDevice->DrawIndexed(6); */

            this->m_Renderer->End();
            this->m_GraphicsDevice->GetSwapchain()->Present();

            this->m_PreviousSize = size;
            
            glm::mat4 model = glm::scale(glm::mat4(1.0f), {100.0f, 100.0f, 100.0f});
            this->m_MVP = model * this->m_Camera.GetProjection();

            /* std::stringstream ss;
            auto memUsage = GetCurrentMemoryUsage();
            ss << "Currently using " << memUsage << " bytes";
            NX_LOG(ss.str()); */
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

        glm::mat4 m_MVP;

        Nexus::OrthographicCamera m_Camera{1000, 1000};
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