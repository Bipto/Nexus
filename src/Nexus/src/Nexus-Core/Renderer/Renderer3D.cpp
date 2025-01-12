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

const std::string c_ModelVertexShader = R"(
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

layout (location = 0) out vec2 OutTexCoord;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec3 FragPos;
layout (location = 3) out vec3 ViewPos;
layout (location = 4) out mat3 TBN;

layout (std140, binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_ViewPos;
};

layout (std140,binding = 1, set = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Transform * vec4(Position, 1.0);
    OutTexCoord = TexCoord;
    OutNormal = mat3(transpose(inverse(u_Transform))) * Normal;
    FragPos = vec3(u_Transform * vec4(Position, 1.0));
    ViewPos = u_ViewPos;

    vec3 T = normalize(vec3(u_Transform * vec4(Tangent, 0.0)));
    vec3 B = normalize(vec3(u_Transform * vec4(Bitangent, 0.0)));
    vec3 N = normalize(vec3(u_Transform * vec4(Normal, 0.0)));
    TBN = mat3(T, B, N);
}
)";

const std::string c_ModelFragmentShader = R"(
#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 1) in vec3 OutNormal;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in vec3 ViewPos;
layout (location = 4) in mat3 TBN;

layout (location = 0) out vec4 FragColor;

layout (binding = 0, set = 1) uniform sampler2D diffuseMapSampler;
layout (binding = 1, set = 1) uniform sampler2D normalMapSampler;
layout (binding = 2, set = 1) uniform sampler2D specularMapSampler;

void main()
{
	vec3 objectColor = texture(diffuseMapSampler, OutTexCoord).rgb;
    FragColor = vec4(objectColor, 1.0);
})";

namespace Nexus::Graphics
{
	Renderer3D::Renderer3D(GraphicsDevice *device) : m_Device(device), m_Camera(m_Device)
	{
		m_CommandList = m_Device->CreateCommandList();

		CreateCubemapPipeline();
		CreateModelPipeline();

		Nexus::Graphics::MeshFactory factory(m_Device);
		m_Cube = factory.CreateCube();
	}

	Renderer3D::~Renderer3D()
	{
	}

	void Nexus::Graphics::Renderer3D::Begin(Scene *scene, RenderTarget target, Nexus::TimeSpan time)
	{
		m_Scene			 = scene;
		m_RenderTarget	 = target;
		m_Cubemap		 = scene->SceneEnvironment.EnvironmentCubemap;
		m_CubemapSampler = scene->SceneEnvironment.CubemapSampler;

		Nexus::Point2D<uint32_t> size = m_RenderTarget.GetSize();
		m_Camera.Update(size.X, size.Y, time);
	}

	void Nexus::Graphics::Renderer3D::End()
	{
		CubemapCameraUniforms cubemapCameraUniforms = {};
		cubemapCameraUniforms.Projection			= m_Camera.GetProjection();
		cubemapCameraUniforms.View					= glm::mat4(glm::mat3(m_Camera.GetView()));
		m_CameraUniformBuffer->SetData(&cubemapCameraUniforms, sizeof(cubemapCameraUniforms));

		ModelCameraUniforms modelCameraUniforms = {};
		modelCameraUniforms.Projection			= m_Camera.GetProjection();
		modelCameraUniforms.View				= m_Camera.GetView();
		modelCameraUniforms.CamPosition			= m_Camera.GetPosition();
		m_ModelCameraUniformBuffer->SetData(&modelCameraUniforms, sizeof(modelCameraUniforms));

		ModelTransformUniforms modelTransformUniforms = {};
		modelTransformUniforms.Transform			  = glm::mat4(1.0f);
		m_ModelTransformUniformBuffer->SetData(&modelTransformUniforms, sizeof(modelTransformUniforms));

		RenderCubemap();
		// for (Nexus::Ref<Nexus::Graphics::Model> model : m_Scene.Models) { RenderModel(model); }
	}

	void Renderer3D::RenderCubemap()
	{
		Nexus::Point2D<uint32_t> size = m_RenderTarget.GetSize();
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

		const Environment &environment = m_Scene->SceneEnvironment;

		m_CommandList->ClearColorTarget(0,
										{environment.ClearColour.r, environment.ClearColour.g, environment.ClearColour.b, environment.ClearColour.a});

		m_CommandList->ClearDepthTarget(Nexus::Graphics::ClearDepthStencilValue {});

		if (m_Cubemap)
		{
			m_CommandList->SetPipeline(m_CubemapPipeline);
			m_CubemapResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, "Camera");
			m_CubemapResourceSet->WriteCombinedImageSampler(m_Cubemap, m_CubemapSampler, "skybox");
			m_CommandList->SetResourceSet(m_CubemapResourceSet);

			m_CommandList->SetVertexBuffer(m_Cube->GetVertexBuffer(), 0);
			m_CommandList->SetIndexBuffer(m_Cube->GetIndexBuffer());
			m_CommandList->DrawIndexed(m_Cube->GetIndexBuffer()->GetCount(), 0, 0);
		}

		m_CommandList->End();

		m_Device->SubmitCommandList(m_CommandList);
	}

	void Renderer3D::RenderModel(Nexus::Ref<Nexus::Graphics::Model> model)
	{
		Nexus::Point2D<uint32_t> size = m_RenderTarget.GetSize();
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

		m_CommandList->SetPipeline(m_ModelPipeline);
		m_ModelResourceSet->WriteUniformBuffer(m_ModelCameraUniformBuffer, "Camera");
		m_ModelResourceSet->WriteUniformBuffer(m_ModelTransformUniformBuffer, "Transform");

		const Nexus::Graphics::Material &mat = model->GetMeshes()[0]->GetMaterial();

		if (mat.DiffuseTexture)
		{
			m_ModelResourceSet->WriteCombinedImageSampler(mat.DiffuseTexture, m_ModelSampler, "diffuseMapSampler");
		}

		if (mat.NormalTexture)
		{
			m_ModelResourceSet->WriteCombinedImageSampler(mat.NormalTexture, m_ModelSampler, "normalMapSampler");
		}

		if (mat.SpecularTexture)
		{
			m_ModelResourceSet->WriteCombinedImageSampler(mat.SpecularTexture, m_ModelSampler, "specularMapSampler");
		}

		m_CommandList->SetResourceSet(m_ModelResourceSet);

		const auto &meshes = model->GetMeshes();
		for (auto mesh : meshes)
		{
			m_CommandList->SetVertexBuffer(mesh->GetVertexBuffer(), 0);
			m_CommandList->SetIndexBuffer(mesh->GetIndexBuffer());
			m_CommandList->DrawIndexed(mesh->GetIndexBuffer()->GetCount(), 0, 0);
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

		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = false;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = false;
		pipelineDescription.DepthStencilDesc.MinDepth				 = 0;
		pipelineDescription.DepthStencilDesc.MaxDepth				 = 1;
		pipelineDescription.DepthFormat								 = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		m_CubemapPipeline	 = m_Device->CreatePipeline(pipelineDescription);
		m_CubemapResourceSet = m_Device->CreateResourceSet(m_CubemapPipeline);

		Nexus::Graphics::BufferDescription uniformBufferDesc = {};
		uniformBufferDesc.Size								 = sizeof(CubemapCameraUniforms);
		uniformBufferDesc.Usage								 = Nexus::Graphics::BufferUsage::Dynamic;
		m_CameraUniformBuffer								 = m_Device->CreateUniformBuffer(uniformBufferDesc, nullptr);

		Nexus::Graphics::SamplerSpecification samplerSpec = {};
		samplerSpec.AddressModeU						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		m_CubemapSampler								  = m_Device->CreateSampler(samplerSpec);
	}

	void Renderer3D::CreateModelPipeline()
	{
		Nexus::Graphics::PipelineDescription pipelineDescription	 = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode	 = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace	 = Nexus::Graphics::FrontFace::Clockwise;
		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		pipelineDescription.VertexModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_ModelVertexShader, "model.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_ModelFragmentShader, "model.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.Layouts						   = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};
		pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}, {"Transform", 0, 1}};
		pipelineDescription.ResourceSetSpec.SampledImages  = {{"diffuseMapSampler", 1, 0}, {"normalMapSampler", 1, 1}, {"specularMapSampler", 1, 2}};

		pipelineDescription.ColourTargetCount		= 1;
		pipelineDescription.ColourFormats[0]		= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourTargetSampleCount = Nexus::Graphics::SampleCount::SampleCount1;

		m_ModelPipeline	   = m_Device->CreatePipeline(pipelineDescription);
		m_ModelResourceSet = m_Device->CreateResourceSet(m_ModelPipeline);

		Nexus::Graphics::BufferDescription cameraBufferDesc = {};
		cameraBufferDesc.Size								= sizeof(ModelCameraUniforms);
		cameraBufferDesc.Usage								= Nexus::Graphics::BufferUsage::Dynamic;
		m_ModelCameraUniformBuffer							= m_Device->CreateUniformBuffer(cameraBufferDesc, nullptr);

		Nexus::Graphics::BufferDescription transformBufferDesc = {};
		transformBufferDesc.Size							   = sizeof(ModelTransformUniforms);
		transformBufferDesc.Usage							   = Nexus::Graphics::BufferUsage::Dynamic;
		m_ModelTransformUniformBuffer						   = m_Device->CreateUniformBuffer(transformBufferDesc, nullptr);

		Nexus::Graphics::SamplerSpecification samplerSpec = {};
		samplerSpec.AddressModeU						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		m_ModelSampler									  = m_Device->CreateSampler(samplerSpec);
	}

}	 // namespace Nexus::Graphics