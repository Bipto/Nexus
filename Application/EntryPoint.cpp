#include <iostream>

#include "Core/Runtime.h"
#include "Core/Application.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Window.h"
#include "Core/Size.h"
#include "Core/Graphics/Swapchain.h"

#include <sstream>

#include "glm.hpp"

/*  float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f
};  */

glm::vec3 vertices[] = {
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f},
    {-0.5f, 0.5f, 0.0f}
};

unsigned int indices[] = {
	0, 1, 2,
	2, 3, 0
};

const char * vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos\n;"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\n";

	const char * fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f)\n;"
	"}\n";

class Editor : public NexusEngine::Application
{
    public:
        Editor() : Application(NexusEngine::GraphicsAPI::OpenGL){}

        void Load() override
        {
            this->m_GraphicsDevice->GetSwapchain()->SetVSyncState(NexusEngine::VSyncState::Disabled);
            this->m_Shader = this->m_GraphicsDevice->CreateShader(vertexShaderSource, fragmentShaderSource);
            this->m_VertexBuffer =  this->m_GraphicsDevice->CreateVertexBuffer(vertices, sizeof(vertices), 3 * sizeof(float), 0, 3);
            this->m_IndexBuffer = this->m_GraphicsDevice->CreateIndexBuffer(indices, sizeof(indices));

            /* glm::vec3 v1 = {0.3f, 0.4f, 0.5f};

            std::ostringstream ss;
            ss << v1.x << ", " << v1.y << ", " << v1.z;

            NexusEngine::GetCoreLogger()->LogMessage(ss.str(), NexusEngine::Severity::Info); */

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
            this->m_VertexBuffer->Bind();
            this->m_IndexBuffer->Bind();
            this->m_GraphicsDevice->DrawIndexed(6);
            //this->m_GraphicsDevice->DrawElements(0, sizeof(vertices) / sizeof(float));

            this->m_GraphicsDevice->GetSwapchain()->Present();

            this->m_PreviousSize = size;
        }

        void Unload()
        {
            
        }
    private:
        NexusEngine::Size m_PreviousSize;
        NexusEngine::Shader* m_Shader;
        NexusEngine::VertexBuffer* m_VertexBuffer;
        NexusEngine::IndexBuffer* m_IndexBuffer;
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