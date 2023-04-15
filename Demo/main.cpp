#include "NexusEngine.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Core/Graphics/ShaderGenerator.h"

std::vector<float> vertices = 
{
    -0.5f, -0.5f, 0.0f,   //bottom left
    -0.5f,  0.5f, 0.0f,   //top left
     0.5f, -0.5f, 0.0f,   //bottom right
     0.5f,  0.5f, 0.0f,   //top right
};

std::vector<unsigned int> indices = 
{
    0, 1, 2,
    1, 3, 2
};

class Demo : public Nexus::Application
{
    public:
        Demo(Nexus::GraphicsAPI api) : Application(api){}

        virtual void Load() override
        {
            Nexus::BufferLayout layout = 
            {
                { Nexus::ShaderDataType::Float3, "TEXCOORD" }
            };

            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertices);
            m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(indices);
            m_GraphicsDevice->Resize(this->GetWindowSize());
            m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Enabled);

            m_Texture = m_GraphicsDevice->CreateTexture("brick.jpg");  

            Nexus::ShaderGenerator generator;

            std::ifstream t("vertex.glsl");
            std::stringstream buffer;
            buffer << t.rdbuf();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("shader.glsl", layout);
        }

        virtual void Update(Nexus::Time time) override
        {
            m_GraphicsDevice->SetContext();
            m_GraphicsDevice->Clear(0.8f, 0.2f, 0.3f, 1.0f);
            m_GraphicsDevice->DrawIndexed(m_VertexBuffer, m_IndexBuffer, m_Shader);

            BeginImGuiRender();
            ImGui::ShowDemoWindow();
            if (ImGui::Begin("Texture"))
            {
                ImGui::Image((ImTextureID)m_Texture->GetHandle(), {200, 200});
                ImGui::End();
            }
            EndImGuiRender();            
            
            m_GraphicsDevice->SwapBuffers();   
        }

        virtual void OnResize(Nexus::Point size) override
        {
            m_GraphicsDevice->SetContext();
            m_GraphicsDevice->Resize(size);
        }

        virtual void Unload() override
        {

        }
    private:
        Nexus::Ref<Nexus::Shader> m_Shader;
        Nexus::Ref<Nexus::VertexBuffer> m_VertexBuffer;
        Nexus::Ref<Nexus::IndexBuffer> m_IndexBuffer;
        Nexus::Ref<Nexus::Texture> m_Texture;
};

int main(int argc, char** argv)
{
    Nexus::GraphicsAPI api = Nexus::GraphicsAPI::OpenGL;

    std::vector<std::string> arguments;
    for (int i = 0; i < argc; i++)
    {
        std::string argument = std::string(argv[i]);
        arguments.push_back(argument);
    }
    
    if (arguments.size() > 1)
        if (arguments[1] == std::string("DX"))    
            api = Nexus::GraphicsAPI::DirectX11;

    Nexus::Init(argc, argv);

    Demo* app = new Demo(api);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
}