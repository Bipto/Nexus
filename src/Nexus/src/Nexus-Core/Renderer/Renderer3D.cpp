#include "Renderer3D.hpp"

#include "Nexus-Core/Graphics/MeshFactory.hpp"

const std::string c_CubemapVertexShader = R"(
#version 450 core

layout(location = 0) in vec3 Position;

layout(location = 0) out vec3 OutTexCoord;

layout(binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    OutTexCoord = Position;
    gl_Position = u_Projection * u_View * vec4(Position, 1.0);
}
)";

const std::string c_CubemapFragmentShader = R"(
#version 450 core

layout(location = 0) in vec3 OutTexCoord;

layout(location = 0) out vec4 FragColor;

layout(binding = 0, set = 1) uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, OutTexCoord);
}
)";

namespace Nexus::Graphics
{
	Renderer3D::Renderer3D(GraphicsDevice *device) : m_Device(device), m_Camera(m_Device)
	{
		m_CommandList = m_Device->CreateCommandList();

		m_Camera.SetPosition(glm::vec3(0, 0, 0));

		CreateCubemapPipeline();

		Nexus::Graphics::BufferDescription uniformBufferDesc = {};
		uniformBufferDesc.Size								 = sizeof(CameraUniforms);
		uniformBufferDesc.Usage								 = Nexus::Graphics::BufferUsage::Dynamic;
		m_CameraUniformBuffer								 = m_Device->CreateUniformBuffer(uniformBufferDesc, nullptr);

		Nexus::Graphics::SamplerSpecification samplerSpec = {};
		samplerSpec.AddressModeU						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		m_Sampler										  = m_Device->CreateSampler(samplerSpec);

		Nexus::Graphics::MeshFactory factory(m_Device);
		m_Cube = factory.CreateCube();
	}

	Renderer3D::~Renderer3D()
	{
	}

	void Nexus::Graphics::Renderer3D::Begin(const Scene &scene, RenderTarget target, Ref<Cubemap> cubemap, Nexus::TimeSpan time)
	{
		m_Scene		   = scene;
		m_RenderTarget = target;
		m_Cubemap	   = cubemap;

		Nexus::Point2D<uint32_t> size = m_RenderTarget.GetSize();
		m_Camera.Update(size.X, size.Y, time);
	}

	void Nexus::Graphics::Renderer3D::End()
	{
		Nexus::Point2D<uint32_t> size = m_RenderTarget.GetSize();

		CameraUniforms cameraUniforms = {};
		cameraUniforms.Projection	  = m_Camera.GetProjection();
		cameraUniforms.View			  = glm::mat4(glm::mat4(m_Camera.GetView()));
		m_CameraUniformBuffer->SetData(&cameraUniforms, sizeof(cameraUniforms));

		m_CommandList->Begin();
		m_CommandList->SetRenderTarget(m_RenderTarget);

		Nexus::Graphics::Viewport vp;
		vp.X		= 0;
		vp.Y		= 0;
		vp.Width	= size.X;
		vp.Height	= size.Y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_CommandList->SetViewport(vp);

		Nexus::Graphics::Scissor scissor;
		scissor.X	   = 0;
		scissor.Y	   = 0;
		scissor.Width  = size.X;
		scissor.Height = size.Y;
		m_CommandList->SetScissor(scissor);

		m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
		m_CommandList->ClearDepthTarget(Nexus::Graphics::ClearDepthStencilValue {});

		if (m_Cubemap)
		{
			m_CommandList->SetPipeline(m_CubemapPipeline);
			m_CubemapResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, "Camera");
			m_CubemapResourceSet->WriteCombinedImageSampler(m_Cubemap, m_Sampler, "skybox");
			m_CommandList->SetResourceSet(m_CubemapResourceSet);

			m_CommandList->SetVertexBuffer(m_Cube->GetVertexBuffer(), 0);
			m_CommandList->SetIndexBuffer(m_Cube->GetIndexBuffer());
			m_CommandList->DrawIndexed(m_Cube->GetIndexBuffer()->GetCount(), 0, 0);
		}

		m_CommandList->End();
		m_Device->SubmitCommandList(m_CommandList);
	}

	void Renderer3D::CreateCubemapPipeline()
	{
		Nexus::Graphics::PipelineDescription pipelineDescription  = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
		pipelineDescription.VertexModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_CubemapVertexShader, "cubemap.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_CubemapFragmentShader, "cubemap.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}};
		pipelineDescription.ResourceSetSpec.SampledImages  = {{"skybox", 1, 0}};

		pipelineDescription.ColourTargetCount		= 1;
		pipelineDescription.ColourFormats[0]		= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourTargetSampleCount = SampleCount::SampleCount1;
		pipelineDescription.Layouts					= {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
		pipelineDescription.DepthStencilDesc.MinDepth				 = 0;
		pipelineDescription.DepthStencilDesc.MaxDepth				 = 1;
		pipelineDescription.DepthFormat								 = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		m_CubemapPipeline	 = m_Device->CreatePipeline(pipelineDescription);
		m_CubemapResourceSet = m_Device->CreateResourceSet(m_CubemapPipeline);
	}

}	 // namespace Nexus::Graphics