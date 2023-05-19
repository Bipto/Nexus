#include "NexusEngine.h"

#include "Core/Graphics/MeshFactory.h"

struct alignas(16) VB_UNIFORM_CAMERA
{
    glm::mat4 View;
    glm::mat4 Projection;
};

struct alignas(16) VB_UNIFORM_RENDERINFO
{
    glm::mat4 Transform;
    glm::vec3 Color;
};

const char* vertexShaderSource = 
"#version 300 es\n"
"layout(std140) uniform Camera\n"
"{\n"
"mat4 u_View;\n"
"mat4 u_Projection;\n"
"}_28;\n"
"layout(std140) uniform RenderInfo\n"
"{\n"
"mat4 u_Transform;\n"
"vec3 u_Color;\n"
"}_23;\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"out vec3 OutColor;\n"
"void main()\n" 
"{\n"
"gl_Position = _28.u_Projection * _28.u_View * _23.u_Transform * vec4(aPos, 1.0);\n"
"TexCoord = aTexCoord;\n"
"OutColor = _23.u_Color;\n"
"}";

std::string fragmentShaderSource = 
"#version 300 es\n"
"precision mediump float;\n"
"precision highp int;\n"
"layout (location = 0) out vec4 FragColor;\n"
"in highp vec2 TexCoord;\n"
"in highp vec3 OutColor;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"FragColor = texture(ourTexture, TexCoord) * vec4(OutColor, 1.0);\n"
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

            Nexus::FramebufferSpecification spec;
            spec.Width = 500;
            spec.Height = 500;
            spec.ColorAttachmentSpecification = { Nexus::TextureFormat::RGBA8 };
            m_Framebuffer = m_GraphicsDevice->CreateFramebuffer(spec);

            m_Texture = m_GraphicsDevice->CreateTexture("brick.jpg");         

            #ifndef __EMSCRIPTEN__
            m_Shader = m_GraphicsDevice->CreateShaderFromSpirvFile("shader.glsl", layout);
            #else
            m_Shader = m_GraphicsDevice->CreateShaderFromSource(vertexShaderSource, fragmentShaderSource, layout);
            #endif

            Nexus::UniformResourceBinding cameraUniformBinding;
            cameraUniformBinding.Binding = 0;
            cameraUniformBinding.Name = "Camera";
            cameraUniformBinding.Size = sizeof(VB_UNIFORM_CAMERA);
            m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBinding);
            m_CameraUniformBuffer->BindToShader(m_Shader);

            Nexus::UniformResourceBinding renderInfoUniformBinding;
            renderInfoUniformBinding.Binding = 1;
            renderInfoUniformBinding.Name = "RenderInfo";
            renderInfoUniformBinding.Size = sizeof(VB_UNIFORM_RENDERINFO);
            m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(renderInfoUniformBinding);
            m_TransformUniformBuffer->BindToShader(m_Shader);

            Nexus::MeshFactory factory(m_GraphicsDevice);
            m_SpriteMesh = factory.CreateCube();
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

            m_CameraUniforms.View = m_Camera.GetView();
            m_CameraUniforms.Projection = m_Camera.GetProjection();           
            m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms), 0);

            m_RenderInfoUniforms.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));
            m_RenderInfoUniforms.Color = m_CubeColor;
            m_TransformUniformBuffer->SetData(&m_RenderInfoUniforms, sizeof(m_RenderInfoUniforms), 0);

            m_GraphicsDevice->Clear(
                m_ClearColor.r,
                m_ClearColor.g,
                m_ClearColor.b,
                1.0f
            );

            Nexus::TextureBinding textureBinding;
            textureBinding.Slot = 1;
            textureBinding.Name = "ourTexture";

            m_Shader->SetTexture(m_Texture, textureBinding);
            m_GraphicsDevice->SetShader(m_Shader);
            m_GraphicsDevice->SetVertexBuffer(m_SpriteMesh.GetVertexBuffer());
            m_GraphicsDevice->SetIndexBuffer(m_SpriteMesh.GetIndexBuffer());
            m_GraphicsDevice->DrawIndexed(Nexus::PrimitiveType::Triangle, m_SpriteMesh.GetIndexBuffer()->GetIndexCount(), 0); 

            m_Camera.Update(
                GetWindowSize().X,
                GetWindowSize().Y,
                time
            );

            if (ImGui::Begin("Settings"));
            ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColor));
            ImGui::ColorEdit3("Cube Colour", glm::value_ptr(m_CubeColor));
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
        Nexus::Ref<Nexus::UniformBuffer> m_CameraUniformBuffer;
        Nexus::Ref<Nexus::UniformBuffer> m_TransformUniformBuffer;
        Nexus::Ref<Nexus::Texture> m_Texture;
        Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;
        Nexus::Mesh m_SpriteMesh;

        Nexus::FirstPersonCamera m_Camera;

        VB_UNIFORM_CAMERA m_CameraUniforms;
        VB_UNIFORM_RENDERINFO m_RenderInfoUniforms;

        glm::vec3 m_ClearColor { 0.8f, 0.2f, 0.3f };
        glm::vec3 m_CubeColor { 1.0f, 1.0f, 1.0f };
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