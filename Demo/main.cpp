#include "NexusEngine.h"

#include <iostream>
#include <fstream>
#include <sstream>

std::vector<float> vertices = 
{
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  //bottom left
    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  //top left
     0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  //bottom right
     0.5f,  0.5f, 0.0f, 0.0f, 1.0f  //top right
};

std::vector<unsigned int> indices = 
{
    0, 1, 2,
    1, 3, 2
};

struct VB_UNIFORM_CAMERA
{
    glm::mat4 ViewProjection;
};

class Demo : public Nexus::Application
{
    public:
        Demo(Nexus::GraphicsAPI api) : Application(api){}

        virtual void Load() override
        {
            Nexus::BufferLayout layout = 
            {
                { Nexus::ShaderDataType::Float3, "TEXCOORD", 0 },
                { Nexus::ShaderDataType::Float2, "TEXCOORD", 1}
            };

            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertices);
            m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(indices);
            m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(sizeof(VB_UNIFORM_CAMERA), 0);
            m_GraphicsDevice->Resize(this->GetWindowSize());
            m_GraphicsDevice->SetVSyncState(Nexus::VSyncState::Enabled);

            Nexus::FramebufferSpecification spec;
            spec.Width = 500;
            spec.Height = 500;
            spec.ColorAttachmentSpecification = { Nexus::TextureFormat::RGBA8 };
            m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec);

            m_Texture = m_GraphicsDevice->CreateTexture("brick.jpg");  

            std::ifstream t("vertex.glsl");
            std::stringstream buffer;
            buffer << t.rdbuf();

            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("shader.glsl", layout);

            m_CameraUniforms.ViewProjection = glm::mat4(1.0f);
            m_UniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);
        }

        virtual void Update(Nexus::Time time) override
        {
            m_GraphicsDevice->SetFramebuffer(m_Framebuffer);
            Nexus::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = m_Framebuffer->GetFramebufferSpecification().Width;
            vp.Height = m_Framebuffer->GetFramebufferSpecification().Height;
            m_GraphicsDevice->SetViewport(vp);
            m_GraphicsDevice->Clear(0.8f, 0.2f, 0.3f, 1.0f);
            m_Shader->SetTexture(m_Texture, 1);
            m_GraphicsDevice->DrawIndexed(m_VertexBuffer, m_IndexBuffer, m_Shader);
            
            Nexus::Viewport vp2;
            vp2.X = 0;
            vp2.Y = 0;
            vp2.Width = this->GetWindowSize().Width;
            vp2.Height = this->GetWindowSize().Height;
            m_GraphicsDevice->SetFramebuffer(nullptr);
            m_GraphicsDevice->SetViewport(vp2);
            m_GraphicsDevice->Clear(0.0f, 0.7f, 0.2f, 1.0f);
            BeginImGuiRender();
            ImGui::ShowDemoWindow();
            if (ImGui::Begin("Texture"))
            {
                auto texture = m_Framebuffer->GetColorAttachment();
                ImGui::Image((ImTextureID)texture, {200, 200});
                ImGui::End();
            }
            EndImGuiRender();            
            
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
        Nexus::Ref<Nexus::IndexBuffer> m_IndexBuffer;
        Nexus::Ref<Nexus::UniformBuffer> m_UniformBuffer;
        Nexus::Ref<Nexus::Texture> m_Texture;
        Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;

        VB_UNIFORM_CAMERA m_CameraUniforms;
};

int main(int argc, char** argv)
{
    Nexus::GraphicsAPI api = Nexus::GraphicsAPI::DirectX11;
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