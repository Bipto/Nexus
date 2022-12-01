#include "NexusEngine.h"

#include <sstream>
#include "glm.hpp"

std::vector<Vertex> vertices1 = {
    {{-1.0f, -1.0f, 0.0f}, {0, 0}},
    {{0.0f, -1.0f, 0.0f}, {1, 0}},
    {{0.0f, 0.0f, 0.0f}, {1, 1}},
    {{-1.0f, 0.0f, 0.0f}, {0, 1}},
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
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
	"}\n";

	const char * fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
	"void main()\n"
	"{\n"
	"	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
	"}\n";

class Editor : public NexusEngine::Application
{
    public:
        Editor() : Application(NexusEngine::GraphicsAPI::OpenGL){}

        void Load() override
        {
            this->m_GraphicsDevice->GetSwapchain()->SetVSyncState(NexusEngine::VSyncState::Enabled);
            this->m_Shader = this->m_GraphicsDevice->CreateShader(vertexShaderSource, fragmentShaderSource);

            this->m_VertexBuffer1 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices1);
            this->m_IndexBuffer1 = this->m_GraphicsDevice->CreateIndexBuffer(indices1, sizeof(indices1));

            this->m_VertexBuffer2 =  this->m_GraphicsDevice->CreateVertexBuffer(vertices2);
            this->m_IndexBuffer2 = this->m_GraphicsDevice->CreateIndexBuffer(indices2, sizeof(indices2));

            /* glm::vec3 v1 = {0.3f, 0.4f, 0.5f};

            std::ostringstream ss;
            ss << v1.x << ", " << v1.y << ", " << v1.z;

            NexusEngine::GetCoreLogger()->LogMessage(ss.str(), NexusEngine::Severity::Info); */

            m_Texture1 = this->m_GraphicsDevice->CreateTexture("brick.jpg");
            m_Texture2 = this->m_GraphicsDevice->CreateTexture("wall.jpg");

            this->m_GraphicsDevice->GetResourceFactory().Print();
        }

        void Update()
        {
            this->m_GraphicsDevice->SetContext();
            NexusEngine::Size size = this->GetWindowSize();

            if (size.Height != this->m_PreviousSize.Height || size.Width != this->m_PreviousSize.Width)
            {
                this->m_GraphicsDevice->GetSwapchain()->Resize(size);
            }

            this->m_GraphicsDevice->Clear(0.07f, 0.13f, 0.17f, 1);

            this->m_Shader->Bind();

            this->m_Texture1->Bind();
            this->m_Shader->SetShaderUniform1i("ourTexture", 0);
            this->m_VertexBuffer1->Bind();
            this->m_IndexBuffer1->Bind();            
            this->m_GraphicsDevice->DrawIndexed(6);

            this->m_Texture2->Bind();
            this->m_Shader->SetShaderUniform1i("ourTexture", 0);
            this->m_VertexBuffer2->Bind();
            this->m_IndexBuffer2->Bind();            
            this->m_GraphicsDevice->DrawIndexed(6);

            this->m_GraphicsDevice->GetSwapchain()->Present();

            this->m_PreviousSize = size;
        }

        void Unload()
        {
            
        }
    private:
        NexusEngine::Size m_PreviousSize;
        NexusEngine::Shader* m_Shader;

        NexusEngine::Texture* m_Texture1;
        NexusEngine::VertexBuffer* m_VertexBuffer1;
        NexusEngine::IndexBuffer* m_IndexBuffer1;

        NexusEngine::Texture* m_Texture2;
        NexusEngine::VertexBuffer* m_VertexBuffer2;
        NexusEngine::IndexBuffer* m_IndexBuffer2;
};

int main(int argc, char** argv)
{
    NexusEngine::Init();

    Editor* app = new Editor();
    NexusEngine::Run(app);
    delete app;

    NexusEngine::Shutdown();
    return 0;
} 