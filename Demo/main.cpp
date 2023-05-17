#include "NexusEngine.h"

#include "Core/Input/Input.h"

std::vector<float> vertices = 
{
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  //bottom left
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  //top left
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  //bottom right
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f   //top right
};

std::vector<unsigned int> indices = 
{
    2, 1, 0,
    2, 3, 1
};

struct VB_UNIFORM_CAMERA
{
    glm::mat4 ViewProjection;
};

const char* vertexShaderSource = 
"#version 300 es\n"
"layout(std140) uniform Camera\n"
"{\n"
"mat4 u_WVP;\n"
"}_28;\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n" 
"{\n"
"gl_Position = vec4(aPos, 1.0) * _28.u_WVP;\n"
"TexCoord = aTexCoord;\n"
"}";

std::string fragmentShaderSource = 
"#version 300 es\n"
"precision mediump float;\n"
"precision highp int;\n"
"layout (location = 0) out vec4 FragColor;\n"
"in highp vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"FragColor = texture(ourTexture, TexCoord);\n"
"}";


class Demo : public Nexus::Application
{
    public:
        Demo(const Nexus::ApplicationSpecification& spec) : Application(spec){}

        virtual void Load() override
        {
            Nexus::VertexBufferLayout layout = 
            {
                { Nexus::ShaderDataType::Float3, "TEXCOORD", 0 },
                { Nexus::ShaderDataType::Float2, "TEXCOORD", 1}
            };

            m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(vertices);
            m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(indices);

            Nexus::FramebufferSpecification spec;
            spec.Width = 500;
            spec.Height = 500;
            spec.ColorAttachmentSpecification = { Nexus::TextureFormat::RGBA8 };
            m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec);

            m_Texture = m_GraphicsDevice->CreateTexture("brick.jpg");  

            std::ifstream t("vertex.glsl");
            std::stringstream buffer;
            buffer << t.rdbuf();            

            #ifndef __EMSCRIPTEN__
            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("shader.glsl", layout);
            #else
            m_Shader = m_GraphicsDevice->CreateShaderFromSource(vertexShaderSource, fragmentShaderSource, layout);
            #endif

            Nexus::UniformResourceBinding binding;
            binding.Binding = 0;
            binding.Name = "Camera";
            binding.Size = sizeof(VB_UNIFORM_CAMERA);

            m_UniformBuffer = m_GraphicsDevice->CreateUniformBuffer(binding);
            m_UniformBuffer->BindToShader(m_Shader);

            m_CameraUniforms.ViewProjection = glm::mat4(1.0f);
            m_UniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            NX_LOG(m_GraphicsDevice->GetAPIName());
            NX_LOG(m_GraphicsDevice->GetDeviceName());
        }

        virtual void Update(Nexus::Time time) override
        {
            
        }

        virtual void Render(Nexus::Time time) override
        {
            m_GraphicsDevice->SetFramebuffer(nullptr);
            Nexus::Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Width = GetWindowSize().X;
            vp.Height = GetWindowSize().Y;
            m_GraphicsDevice->SetViewport(vp);

            float aspectRatio = (float)GetWindowSize().X / (float)GetWindowSize().Y;

            m_CameraUniforms.ViewProjection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f);
            m_UniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_GraphicsDevice->Clear( 0.8f, 0.2f, 0.3f, 1.0f );

            Nexus::TextureBinding textureBinding;
            textureBinding.Slot = 1;
            textureBinding.Name = "ourTexture";

            m_Shader->SetTexture(m_Texture, textureBinding);
            m_GraphicsDevice->SetShader(m_Shader);
            m_GraphicsDevice->SetVertexBuffer(m_VertexBuffer);
            m_GraphicsDevice->SetIndexBuffer(m_IndexBuffer);
            m_GraphicsDevice->DrawIndexed(Nexus::PrimitiveType::Triangle, m_IndexBuffer->GetIndexCount(), 0); 

            ImGui::Begin("Info");
            
            std::string width = std::to_string(GetWindowSize().X);
            std::string height = std::to_string(GetWindowSize().Y);

            ImGui::Text(width.c_str());
            ImGui::Text(height.c_str());

            ImGui::End();
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
    Nexus::ApplicationSpecification spec;
    spec.API = Nexus::GraphicsAPI::OpenGL;
    spec.ImGuiActive = true;
    spec.VSyncState = Nexus::VSyncState::Enabled;
    spec.UpdatesPerSecond = 1;
    spec.RendersPerSecond = 30;

    std::vector<std::string> arguments;
    for (int i = 0; i < argc; i++)
    {
        std::string argument = std::string(argv[i]);
        arguments.push_back(argument);
    }
    
    if (arguments.size() > 1)
        if (arguments[1] == std::string("DX"))    
            spec.API = Nexus::GraphicsAPI::DirectX11;

    Nexus::Init(argc, argv);

    Demo* app = new Demo(spec);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
}