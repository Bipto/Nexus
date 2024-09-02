#pragma once

#include "Demo.hpp"

#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace Demos
{
struct alignas(16) VB_UNIFORM_CAMERA_DEMO_3D
{
    glm::mat4 View;
    glm::mat4 Projection;
};

struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_3D
{
    glm::mat4 Transform;
};

class Demo3D : public Demo
{
  public:
    Demo3D(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer) : Demo(name, app, imGuiRenderer)
    {
    }

    virtual ~Demo3D()
    {
    }

    virtual void Load() override
    {
        m_CommandList = m_GraphicsDevice->CreateCommandList();
        Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
        m_Mesh = factory.CreateCube();
        m_Texture = m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_diff_1k.jpg").c_str(), true);

        Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
        cameraUniformBufferDesc.Size = sizeof(VB_UNIFORM_CAMERA_DEMO_3D);
        cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_CameraUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);

        Nexus::Graphics::BufferDescription transformUniformBufferDesc;
        transformUniformBufferDesc.Size = sizeof(VB_UNIFORM_TRANSFORM_DEMO_3D);
        transformUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_TransformUniformBuffer = m_GraphicsDevice->CreateUniformBuffer(transformUniformBufferDesc, nullptr);

        CreatePipeline();

        Nexus::Graphics::SamplerSpecification samplerSpec{};
        m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
    }

    virtual void Render(Nexus::Time time) override
    {
        m_TransformUniforms.Transform = glm::rotate(glm::mat4(1.0f), glm::radians((float)m_ElapsedTime.GetSeconds() * 100.0f), glm::vec3(0.0f, 1.0f, 1.0f));
        m_TransformUniformBuffer->SetData(&m_TransformUniforms, sizeof(m_TransformUniforms));

        m_CameraUniforms.View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f));
        m_CameraUniforms.Projection = glm::perspectiveFov<float>(glm::radians(60.0f), m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y, 0.1f, 100.0f);
        m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms));

        m_CommandList->Begin();
        m_CommandList->SetPipeline(m_Pipeline);
        m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()));

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
        vp.Height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        m_CommandList->SetViewport(vp);

        Nexus::Graphics::Scissor scissor;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().X;
        scissor.Height = m_GraphicsDevice->GetPrimaryWindow()->GetWindowSize().Y;
        m_CommandList->SetScissor(scissor);

        m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

        Nexus::Graphics::ClearDepthStencilValue clearValue;
        m_CommandList->ClearDepthTarget(clearValue);

        m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, "Camera");
        m_ResourceSet->WriteUniformBuffer(m_TransformUniformBuffer, "Transform");
        m_ResourceSet->WriteCombinedImageSampler(m_Texture, m_Sampler, "texSampler");

        m_CommandList->SetResourceSet(m_ResourceSet);

        m_CommandList->SetVertexBuffer(m_Mesh->GetVertexBuffer(), 0);
        m_CommandList->SetIndexBuffer(m_Mesh->GetIndexBuffer());

        auto indexCount = m_Mesh->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
        m_CommandList->DrawIndexed(indexCount, 0, 0);
        m_CommandList->End();

        m_GraphicsDevice->SubmitCommandList(m_CommandList);

        m_ElapsedTime = m_ElapsedTime.GetNanoseconds() + time.GetNanoseconds();
    }

  private:
    void CreatePipeline()
    {
        Nexus::Graphics::PipelineDescription pipelineDescription;
        pipelineDescription.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::Back;
        pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

        pipelineDescription.VertexModule = m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/demo/shaders/3d.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
        pipelineDescription.FragmentModule = m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/demo/shaders/3d.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

        pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}, {"Transform", 0, 1}};

        pipelineDescription.ResourceSetSpec.SampledImages = {{"texSampler", 1, 0}};

        pipelineDescription.ColourTargetCount = 1;
        pipelineDescription.ColourFormats[0] = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
        pipelineDescription.ColourTargetSampleCount = m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()->GetSpecification().Samples;

        pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

        m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
        m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
    }

  private:
    Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
    Nexus::Ref<Nexus::Graphics::Pipeline> m_Pipeline;
    Nexus::Ref<Nexus::Graphics::Mesh> m_Mesh;
    Nexus::Ref<Nexus::Graphics::Texture2D> m_Texture;
    Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler;
    glm::vec3 m_ClearColour = {0.7f, 0.2f, 0.3f};

    Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;

    VB_UNIFORM_CAMERA_DEMO_3D m_CameraUniforms;
    Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

    VB_UNIFORM_TRANSFORM_DEMO_3D m_TransformUniforms;
    Nexus::Ref<Nexus::Graphics::UniformBuffer> m_TransformUniformBuffer;

    Nexus::Time m_ElapsedTime = 0;
};
} // namespace Demos