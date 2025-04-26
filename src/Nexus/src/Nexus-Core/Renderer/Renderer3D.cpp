#include "Nexus-Core/Renderer/Renderer3D.hpp"

#include "Nexus-Core/ECS/Components.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"

const std::string c_ClearGBufferVertexShader = R"(
#version 450 core

layout (location = 0) in vec3 Position;

void main()
{
	gl_Position = vec4(Position, 1.0);
}

)";

const std::string c_ClearGBufferFragmentShader = R"(

#version 450 core

layout (location = 0) out vec4 Diffuse;
layout (location = 1) out uvec2 ID; 

void main()
{
	Diffuse = vec4(0.0, 0.0, 0.0, 0.0);
	ID = uvec2(0, 0);
}

)";

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
layout(location = 1) out uvec2 EntityID;

layout(binding = 0, set = 1) uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, OutTexCoord);
	EntityID = uvec2(0, 0);
}
)";

const std::string c_ModelVertexShader = R"(
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec4 VertexColour;
layout (location = 4) in vec3 Tangent;
layout (location = 5) in vec3 Bitangent;

layout (location = 0) out vec2 OutTexCoord;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec3 FragPos;
layout (location = 3) out vec4 VertexDiffuseColour;
layout (location = 4) out vec4 MaterialDiffuseColour;
layout (location = 5) out vec3 ViewPos;
layout (location = 6) flat out uvec2 EntityID;
layout (location = 7) out mat3 TBN;

layout (std140, binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_ViewPos;
};

layout (std140,binding = 1, set = 0) uniform Transform
{
    mat4 u_Transform;
	vec4 u_DiffuseColour;
	vec4 u_SpecularColour;
	uint u_Guid1;
	uint u_Guid2;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Transform * vec4(Position, 1.0);
    OutTexCoord = TexCoord;
    OutNormal = mat3(transpose(inverse(u_Transform))) * Normal;
    FragPos = vec3(u_Transform * vec4(Position, 1.0));
    ViewPos = u_ViewPos;

	VertexDiffuseColour = VertexColour;
	MaterialDiffuseColour = u_DiffuseColour;

    vec3 T = normalize(vec3(u_Transform * vec4(Tangent, 0.0)));
    vec3 B = normalize(vec3(u_Transform * vec4(Bitangent, 0.0)));
    vec3 N = normalize(vec3(u_Transform * vec4(Normal, 0.0)));
    TBN = mat3(T, B, N);

	EntityID = uvec2(u_Guid1, u_Guid2);
}
)";

const std::string c_ModelFragmentShader = R"(
#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 1) in vec3 OutNormal;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in vec4 VertexDiffuseColour;
layout (location = 4) in vec4 MaterialDiffuseColour;
layout (location = 5) in vec3 ViewPos;
layout (location = 6) flat in uvec2 EntityID;
layout (location = 7) in mat3 TBN;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out uvec2 o_EntityID;

layout (binding = 0, set = 1) uniform sampler2D diffuseMapSampler;
layout (binding = 1, set = 1) uniform sampler2D normalMapSampler;
layout (binding = 2, set = 1) uniform sampler2D specularMapSampler;

void main()
{
	vec4 objectColor = texture(diffuseMapSampler, OutTexCoord) * VertexDiffuseColour * MaterialDiffuseColour;
    FragColor = vec4(objectColor.rgb, objectColor.a);
	o_EntityID = EntityID;
})";

namespace Nexus::Graphics
{
	Renderer3D::Renderer3D(GraphicsDevice *device) : m_Device(device), m_Camera(m_Device), m_FullscreenQuad(m_Device, false)
	{
		m_CommandList = m_Device->CreateCommandList();

		CreateClearGBufferPipeline();
		CreateCubemapPipeline();
		CreateModelPipeline();

		Nexus::Graphics::MeshFactory factory(m_Device);
		m_Cube = factory.CreateCube();

		Nexus::Graphics::Texture2DSpecification textureSpec = {};
		textureSpec.Width									= 1;
		textureSpec.Height									= 1;
		textureSpec.Format									= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		m_DefaultTexture									= m_Device->CreateTexture2D(textureSpec);

		uint32_t colour = 0xFFFFFFFF;
		m_DefaultTexture->SetData(&colour, 0, 0, 0, 1, 1);
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
		m_CubemapUniformBuffer->SetData(&cubemapCameraUniforms, 0, sizeof(cubemapCameraUniforms));

		ModelCameraUniforms modelCameraUniforms = {};
		modelCameraUniforms.Projection			= m_Camera.GetProjection();
		modelCameraUniforms.View				= m_Camera.GetView();
		modelCameraUniforms.CamPosition			= m_Camera.GetPosition();
		m_ModelCameraUniformBuffer->SetData(&modelCameraUniforms, 0, sizeof(modelCameraUniforms));

		ClearGBuffer();
		RenderCubemap();

		ECS::View<Transform, ModelRenderer> transformsModelRenderers = m_Scene->Registry.GetView<Transform, ModelRenderer>();
		transformsModelRenderers.Each(
			[&](Entity *entity, const std::tuple<Transform *, ModelRenderer *> &components)
			{
				Transform	  *transform	 = std::get<0>(components);
				ModelRenderer *modelRenderer = std::get<1>(components);

				if (modelRenderer->Model)
				{
					RenderModel(modelRenderer->Model, transform->CreateTransformation(), entity->ID);
				}
			});
	}

	const Nexus::FirstPersonCamera Renderer3D::GetCamera() const
	{
		return m_Camera;
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
		m_CommandList->ClearColorTarget(1, {0.0f, 0.0f, 0.0f, 0.0f});

		m_CommandList->ClearDepthTarget(Nexus::Graphics::ClearDepthStencilValue {});

		if (m_Cubemap)
		{
			m_CommandList->SetPipeline(m_CubemapPipeline);

			UniformBufferView uniformBufferView = {};
			uniformBufferView.BufferHandle		= m_CubemapUniformBuffer.get();
			uniformBufferView.Offset			= 0;
			uniformBufferView.Size				= m_CubemapUniformBuffer->GetDescription().SizeInBytes;
			m_CubemapResourceSet->WriteUniformBuffer(uniformBufferView, "Camera");

			m_CubemapResourceSet->WriteCombinedImageSampler(m_Cubemap, m_CubemapSampler, "skybox");
			m_CommandList->SetResourceSet(m_CubemapResourceSet);

			Ref<DeviceBuffer> vertexBuffer	   = m_Cube->GetVertexBuffer();
			VertexBufferView  vertexBufferView = {};
			vertexBufferView.BufferHandle	   = vertexBuffer.get();
			vertexBufferView.Offset			   = 0;
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Ref<DeviceBuffer> indexBuffer	  = m_Cube->GetIndexBuffer();
			IndexBufferView	  indexBufferView = {};
			indexBufferView.BufferHandle	  = indexBuffer.get();
			indexBufferView.Offset			  = 0;
			indexBufferView.BufferFormat	  = Graphics::IndexBufferFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			m_CommandList->DrawIndexed(m_Cube->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
		}

		m_CommandList->End();

		m_Device->SubmitCommandList(m_CommandList);
	}

	void Renderer3D::RenderModel(Nexus::Ref<Nexus::Graphics::Model> model, const glm::mat4 transform, GUID guid)
	{
		if (model->GetMeshes().empty())
		{
			return;
		}

		std::pair<uint32_t, uint32_t> splitId = guid.Split();

		for (const auto &mesh : model->GetMeshes())
		{
			const Nexus::Graphics::Material &mat = mesh->GetMaterial();

			ModelTransformUniforms modelTransformUniforms = {};
			modelTransformUniforms.Transform			  = transform;
			modelTransformUniforms.Guid1				  = splitId.first;
			modelTransformUniforms.Guid2				  = splitId.second;
			modelTransformUniforms.DiffuseColour		  = mat.DiffuseColour;
			modelTransformUniforms.SpecularColour		  = mat.SpecularColour;
			m_ModelTransformUniformBuffer->SetData(&modelTransformUniforms, 0, sizeof(modelTransformUniforms));

			Nexus::Ref<Nexus::Graphics::Texture2D> diffuseTexture  = m_DefaultTexture;
			Nexus::Ref<Nexus::Graphics::Texture2D> normalTexture   = m_DefaultTexture;
			Nexus::Ref<Nexus::Graphics::Texture2D> specularTexture = m_DefaultTexture;

			if (mat.DiffuseTexture)
			{
				diffuseTexture = mat.DiffuseTexture;
			}

			if (mat.NormalTexture)
			{
				normalTexture = mat.NormalTexture;
			}

			if (mat.SpecularTexture)
			{
				specularTexture = mat.SpecularTexture;
			}

			m_ModelResourceSet->WriteCombinedImageSampler(diffuseTexture, m_ModelSampler, "diffuseMapSampler");
			m_ModelResourceSet->WriteCombinedImageSampler(normalTexture, m_ModelSampler, "normalMapSampler");
			m_ModelResourceSet->WriteCombinedImageSampler(specularTexture, m_ModelSampler, "specularMapSampler");

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

			UniformBufferView modelCameraUniformView = {};
			modelCameraUniformView.BufferHandle		 = m_ModelCameraUniformBuffer.get();
			modelCameraUniformView.Offset			 = 0;
			modelCameraUniformView.Size				 = m_ModelCameraUniformBuffer->GetDescription().SizeInBytes;
			m_ModelResourceSet->WriteUniformBuffer(modelCameraUniformView, "Camera");

			UniformBufferView modelTransformUniformView = {};
			modelTransformUniformView.BufferHandle		= m_ModelTransformUniformBuffer.get();
			modelTransformUniformView.Offset			= 0;
			modelTransformUniformView.Size				= m_ModelTransformUniformBuffer->GetDescription().SizeInBytes;
			m_ModelResourceSet->WriteUniformBuffer(modelTransformUniformView, "Transform");

			m_CommandList->SetResourceSet(m_ModelResourceSet);

			Ref<DeviceBuffer> vertexBuffer	   = mesh->GetVertexBuffer();
			VertexBufferView  vertexBufferView = {};
			vertexBufferView.BufferHandle	   = vertexBuffer.get();
			vertexBufferView.Offset			   = 0;
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Ref<DeviceBuffer> indexBuffer	  = mesh->GetIndexBuffer();
			IndexBufferView	  indexBufferView = {};
			indexBufferView.BufferHandle	  = indexBuffer.get();
			indexBufferView.Offset			  = 0;
			indexBufferView.BufferFormat	  = Graphics::IndexBufferFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);
			m_CommandList->DrawIndexed(mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

			m_CommandList->End();
			m_Device->SubmitCommandList(m_CommandList);
		}
	}

	void Renderer3D::ClearGBuffer()
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

		m_CommandList->SetPipeline(m_ClearScreenPipeline);
		m_CommandList->Draw(6, 1, 0, 0);

		m_CommandList->End();
		m_Device->SubmitCommandList(m_CommandList);
	}

	void Renderer3D::CreateCubemapPipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
		pipelineDescription.VertexModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_CubemapVertexShader, "cubemap.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_CubemapFragmentShader, "cubemap.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}};
		pipelineDescription.ResourceSetSpec.SampledImages  = {{"skybox", 1, 0}};

		pipelineDescription.ColourTargetCount		= 2;
		pipelineDescription.ColourFormats[0]		= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourFormats[1]		= Nexus::Graphics::PixelFormat::R32_G32_UInt;
		pipelineDescription.ColourTargetSampleCount = 1;
		pipelineDescription.Layouts					= {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = false;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = false;
		pipelineDescription.DepthStencilDesc.MinDepth				 = 0;
		pipelineDescription.DepthStencilDesc.MaxDepth				 = 1;
		pipelineDescription.DepthFormat								 = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		m_CubemapPipeline	 = m_Device->CreatePipeline(pipelineDescription);
		m_CubemapResourceSet = m_Device->CreateResourceSet(m_CubemapPipeline);

		DeviceBufferDescription cubemapBufferDesc = {};
		cubemapBufferDesc.Type					  = DeviceBufferType::Uniform;
		cubemapBufferDesc.StrideInBytes			  = sizeof(CubemapCameraUniforms);
		cubemapBufferDesc.SizeInBytes			  = sizeof(CubemapCameraUniforms);
		cubemapBufferDesc.HostVisible			  = true;
		m_CubemapUniformBuffer					  = Ref<DeviceBuffer>(m_Device->CreateDeviceBuffer(cubemapBufferDesc));

		Nexus::Graphics::SamplerSpecification samplerSpec = {};
		samplerSpec.AddressModeU						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		m_CubemapSampler								  = m_Device->CreateSampler(samplerSpec);
	}

	void Renderer3D::CreateModelPipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode	 = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace	 = Nexus::Graphics::FrontFace::CounterClockwise;
		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		pipelineDescription.VertexModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_ModelVertexShader, "model.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_ModelFragmentShader, "model.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.Layouts						   = {Nexus::Graphics::VertexPositionTexCoordNormalColourTangentBitangent::GetLayout()};
		pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}, {"Transform", 0, 1}};
		pipelineDescription.ResourceSetSpec.SampledImages  = {{"diffuseMapSampler", 1, 0}, {"normalMapSampler", 1, 1}, {"specularMapSampler", 1, 2}};

		pipelineDescription.ColourTargetCount		= 2;
		pipelineDescription.ColourFormats[0]		= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourFormats[1]		= Nexus::Graphics::PixelFormat::R32_G32_UInt;
		pipelineDescription.ColourTargetSampleCount = 1;

		pipelineDescription.ColourBlendStates[0].EnableBlending			= true;
		pipelineDescription.ColourBlendStates[0].SourceColourBlend		= Nexus::Graphics::BlendFactor::SourceAlpha;
		pipelineDescription.ColourBlendStates[0].DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDescription.ColourBlendStates[0].ColorBlendFunction		= Nexus::Graphics::BlendEquation::Add;
		pipelineDescription.ColourBlendStates[0].SourceAlphaBlend		= Nexus::Graphics::BlendFactor::One;
		pipelineDescription.ColourBlendStates[0].DestinationAlphaBlend	= Nexus::Graphics::BlendFactor::Zero;
		pipelineDescription.ColourBlendStates[0].AlphaBlendFunction		= Nexus::Graphics::BlendEquation::Add;

		m_ModelPipeline	   = m_Device->CreatePipeline(pipelineDescription);
		m_ModelResourceSet = m_Device->CreateResourceSet(m_ModelPipeline);

		// model camera
		{
			DeviceBufferDescription cameraBufferDesc = {};
			cameraBufferDesc.Type					 = DeviceBufferType::Uniform;
			cameraBufferDesc.StrideInBytes			 = sizeof(ModelCameraUniforms);
			cameraBufferDesc.SizeInBytes			 = sizeof(ModelCameraUniforms);
			cameraBufferDesc.HostVisible			 = true;
			m_ModelCameraUniformBuffer				 = Ref<DeviceBuffer>(m_Device->CreateDeviceBuffer(cameraBufferDesc));
		}

		// model transform
		{
			DeviceBufferDescription transformBufferDesc = {};
			transformBufferDesc.Type					= DeviceBufferType::Uniform;
			transformBufferDesc.StrideInBytes			= sizeof(ModelTransformUniforms);
			transformBufferDesc.SizeInBytes				= sizeof(ModelTransformUniforms);
			transformBufferDesc.HostVisible				= true;
			m_ModelTransformUniformBuffer				= Ref<DeviceBuffer>(m_Device->CreateDeviceBuffer(transformBufferDesc));
		}

		Nexus::Graphics::SamplerSpecification samplerSpec = {};
		samplerSpec.AddressModeU						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						  = Nexus::Graphics::SamplerAddressMode::Clamp;
		m_ModelSampler									  = m_Device->CreateSampler(samplerSpec);
	}

	void Renderer3D::CreateClearGBufferPipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode	 = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace	 = Nexus::Graphics::FrontFace::Clockwise;
		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		pipelineDescription.VertexModule   = m_Device->GetOrCreateCachedShaderFromSpirvSource(c_ClearGBufferVertexShader,
																							  "clearscreen.vert.glsl",
																							  Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule = m_Device->GetOrCreateCachedShaderFromSpirvSource(c_ClearGBufferFragmentShader,
																							  "clearscreen.frag.glsl",
																							  Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.Layouts						   = {m_FullscreenQuad.GetVertexBufferLayout()};
		pipelineDescription.ResourceSetSpec.UniformBuffers = {};
		pipelineDescription.ResourceSetSpec.SampledImages  = {};

		pipelineDescription.ColourTargetCount		= 2;
		pipelineDescription.ColourFormats[0]		= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourFormats[1]		= Nexus::Graphics::PixelFormat::R32_G32_UInt;
		pipelineDescription.ColourTargetSampleCount = 1;

		pipelineDescription.ColourBlendStates[0].EnableBlending			= true;
		pipelineDescription.ColourBlendStates[0].SourceColourBlend		= Nexus::Graphics::BlendFactor::SourceAlpha;
		pipelineDescription.ColourBlendStates[0].DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDescription.ColourBlendStates[0].ColorBlendFunction		= Nexus::Graphics::BlendEquation::Add;
		pipelineDescription.ColourBlendStates[0].SourceAlphaBlend		= Nexus::Graphics::BlendFactor::One;
		pipelineDescription.ColourBlendStates[0].DestinationAlphaBlend	= Nexus::Graphics::BlendFactor::Zero;
		pipelineDescription.ColourBlendStates[0].AlphaBlendFunction		= Nexus::Graphics::BlendEquation::Add;

		pipelineDescription.ColourBlendStates[1].EnableBlending = false;

		m_ClearScreenPipeline = m_Device->CreatePipeline(pipelineDescription);
	}

}	 // namespace Nexus::Graphics