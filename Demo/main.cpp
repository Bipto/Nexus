#include "NexusEngine.h"

std::vector<float> vertices = 
{
    -0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
};

class Demo : public Nexus::Application
{
    public:
        Demo(Nexus::GraphicsAPI api) : Application(api){}

        virtual void Load() override
        {
            Nexus::BufferLayout layout = 
            {
                { Nexus::ShaderDataType::Float3, "POSITION" }
            };

            if (m_GraphicsDevice->GetGraphicsAPI() == Nexus::GraphicsAPI::DirectX11)
                m_Shader = m_GraphicsDevice->CreateShaderFromFile("shaders.hlsl", layout);
            else
                m_Shader = m_GraphicsDevice->CreateShaderFromFile("shaders.glsl", layout);

            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertices);
            m_GraphicsDevice->Resize(this->GetWindowSize());
            m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Disabled);
        }

        virtual void Update(Nexus::Time time) override
        {
            m_GraphicsDevice->Clear(0.8f, 0.2f, 0.3f, 1.0f);
            m_GraphicsDevice->DrawElements(m_VertexBuffer, m_Shader);
            m_GraphicsDevice->SwapBuffers();   
        }

        virtual void OnResize(Nexus::Point size) override
        {
            m_GraphicsDevice->Resize(size);
        }

        virtual void Unload() override
        {

        }
    private:
        Nexus::Ref<Nexus::Shader> m_Shader;
        Nexus::Ref<Nexus::VertexBuffer> m_VertexBuffer;
};

int main(int argc, char** argv)
{
    Nexus::Init();

    Demo* app = new Demo(Nexus::GraphicsAPI::DirectX11);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
}