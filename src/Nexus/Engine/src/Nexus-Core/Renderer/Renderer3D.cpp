#include "Nexus-Core/Renderer/Renderer3D.hpp"

#include "Nexus-Core/ECS/Components.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Nexus-Core/Utils/GraphicsUtils.hpp"

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
	Renderer3D::Renderer3D(IGraphicsDevice *device, Ref<Graphics::ICommandQueue> commandQueue)
		: m_Device(device),
		  m_CommandQueue(commandQueue),
		  m_Camera(m_Device),
		  m_FullscreenQuad(m_Device, commandQueue, false)
	{
		m_CommandList = m_CommandQueue->CreateCommandList();

		CreateClearGBufferPipeline();
		CreateCubemapPipeline();
		CreateModelPipeline();

		Nexus::Graphics::MeshFactory factory(m_Device, m_CommandQueue);
		m_Cube = factory.CreateCube();

		Graphics::TextureDescription textureDesc = {};
		textureDesc.Width						 = 1;
		textureDesc.Height						 = 1;
		textureDesc.Format						 = PixelFormat::R8_G8_B8_A8_UNorm;
		textureDesc.Usage						 = Graphics::TextureUsage_Sampled;
		textureDesc.DebugName					 = "Default Texture";
		m_DefaultTexture						 = m_Device->CreateTexture(textureDesc);

		uint32_t colour = 0xFFFFFFFF;
		Utils::WriteToTexture(m_CommandQueue, m_DefaultTexture, 0, 0, 0, 0, 1, 1, &colour, sizeof(colour));

		Graphics::TextureViewDescription viewDesc = {};
		viewDesc.TargetTexture					  = m_DefaultTexture;
		viewDesc.Format							  = m_DefaultTexture->GetPixelFormat();
		viewDesc.Range							  = {.BaseMipLevel = 0, .LevelCount = 1, .BaseArrayLayer = 0, .LayerCount = 1};
		viewDesc.DebugName						  = "Default Texture View";
		m_DefaultTextureView					  = m_Device->CreateTextureView(viewDesc);
	}

	Renderer3D::~Renderer3D()
	{
	}

	void Nexus::Graphics::Renderer3D::Begin(Scene *scene, FramebufferHandle target, Nexus::TimeSpan time)
	{
		m_Scene			 = scene;
		m_RenderTarget	 = target;
		m_Cubemap		 = scene->SceneEnvironment.EnvironmentCubemap;
		m_CubemapSampler = scene->SceneEnvironment.CubemapSampler;

		auto [width, height] = m_RenderTarget->GetSize();
		m_Camera.Update(width, height, time);
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

		auto [width, height] = m_RenderTarget->GetSize();
		m_CommandList->Begin();
		m_CommandList->SetFramebuffer(m_RenderTarget);

		Nexus::Graphics::Viewport vp;
		vp.X		= 0;
		vp.Y		= 0;
		vp.Width	= width;
		vp.Height	= height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_CommandList->SetViewport(vp);

		Nexus::Graphics::Scissor scissor;
		scissor.X	   = 0;
		scissor.Y	   = 0;
		scissor.Width  = width;
		scissor.Height = height;
		m_CommandList->SetScissor(scissor);

		m_CommandList->SetPipeline(m_ModelPipeline);

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

		m_CommandList->End();
		m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
		m_Device->WaitForIdle();
	}

	const Nexus::FirstPersonCamera Renderer3D::GetCamera() const
	{
		return m_Camera;
	}

	void Renderer3D::RenderCubemap()
	{
		auto [width, height] = m_RenderTarget->GetSize();
		m_CommandList->Begin();
		m_CommandList->SetFramebuffer(m_RenderTarget);

		Nexus::Graphics::Viewport vp;
		vp.X		= 0;
		vp.Y		= 0;
		vp.Width	= width;
		vp.Height	= height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_CommandList->SetViewport(vp);

		Nexus::Graphics::Scissor scissor;
		scissor.X	   = 0;
		scissor.Y	   = 0;
		scissor.Width  = width;
		scissor.Height = height;
		m_CommandList->SetScissor(scissor);

		const Environment &environment = m_Scene->SceneEnvironment;

		if (m_Cubemap.IsValid())
		{
			m_CommandList->SetPipeline(m_CubemapPipeline);

			UniformBufferView uniformBufferView = {};
			uniformBufferView.BufferHandle		= m_CubemapUniformBuffer;
			uniformBufferView.Offset			= 0;
			uniformBufferView.Size				= m_CubemapUniformBuffer->GetDescription().SizeInBytes;
			m_CubemapResourceSet->WriteUniformBuffer(uniformBufferView, "Camera");

			m_CubemapResourceSet->Flush();

			CombinedImageSampler skyboxCiSampelr = {.ImageTexture = environment.EnvironmentCubemap, .ImageSampler = environment.CubemapSampler};

			Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
			resourceBindingDesc.TargetResourceSet							   = m_CubemapResourceSet;
			resourceBindingDesc.DynamicOffsets								   = {};
			m_CommandList->SetResourceSet(resourceBindingDesc);

			Ref<IDeviceBuffer> vertexBuffer		= m_Cube->GetVertexBuffer();
			VertexBufferView   vertexBufferView = {};
			vertexBufferView.BufferHandle		= vertexBuffer;
			vertexBufferView.Offset				= 0;
			vertexBufferView.Size				= vertexBuffer->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Ref<IDeviceBuffer> indexBuffer	   = m_Cube->GetIndexBuffer();
			IndexBufferView	   indexBufferView = {};
			indexBufferView.BufferHandle	   = indexBuffer;
			indexBufferView.Offset			   = 0;
			indexBufferView.Size			   = indexBuffer->GetSizeInBytes();
			indexBufferView.BufferFormat	   = Graphics::IndexFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			DrawIndexedDescription drawDesc = {};
			drawDesc.VertexStart			= 0;
			drawDesc.IndexStart				= 0;
			drawDesc.InstanceStart			= 0;
			drawDesc.IndexCount				= m_Cube->GetIndexBuffer()->GetCount();
			drawDesc.InstanceCount			= 1;
			m_CommandList->DrawIndexed(drawDesc);
		}

		m_CommandList->End();

		m_CommandQueue->SubmitCommandList(m_CommandList);
		m_CommandQueue->WaitForIdle();
	}	 // namespace Nexus::Graphics

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

			// create the uniform buffer if needed
			{
				if (m_ModelTransformUniformBuffers.find(model) == m_ModelTransformUniformBuffers.end())
				{
					DeviceBufferDescription transformBufferDesc = {};
					transformBufferDesc.Access					= Graphics::BufferMemoryAccess::Upload;
					transformBufferDesc.Usage					= Graphics::BufferUsage_Uniform;
					transformBufferDesc.StrideInBytes			= sizeof(ModelTransformUniforms);
					transformBufferDesc.SizeInBytes				= sizeof(ModelTransformUniforms);
					Ref<IDeviceBuffer> transformUniformBuffer	= m_Device->CreateDeviceBuffer(transformBufferDesc);
					m_ModelTransformUniformBuffers[model]		= transformUniformBuffer;
				}
			}

			// create the resource set if needed
			{
				if (m_ModelResourceSets.find(model) == m_ModelResourceSets.end())
				{
					ResourceSetHandle resourceSet = m_Device->CreateResourceSet(m_ModelPipeline);
					m_ModelResourceSets[model]	  = resourceSet;
				}
			}

			Ref<IDeviceBuffer> transformUniformBuffer = m_ModelTransformUniformBuffers[model];
			ResourceSetHandle  resourceSet			  = m_ModelResourceSets[model];

			// copy data into the uniform buffer
			{
				ModelTransformUniforms modelTransformUniforms = {};
				modelTransformUniforms.Transform			  = transform;
				modelTransformUniforms.Guid1				  = splitId.first;
				modelTransformUniforms.Guid2				  = splitId.second;
				modelTransformUniforms.DiffuseColour		  = mat.DiffuseColour;
				modelTransformUniforms.SpecularColour		  = mat.SpecularColour;
				transformUniformBuffer->SetData(&modelTransformUniforms, 0, sizeof(modelTransformUniforms));
			}

			Graphics::TextureViewHandle diffuseTexture	= m_DefaultTextureView;
			Graphics::TextureViewHandle normalTexture	= m_DefaultTextureView;
			Graphics::TextureViewHandle specularTexture = m_DefaultTextureView;

			if (mat.DiffuseTexture.IsValid())
			{
				diffuseTexture = mat.DiffuseTexture;
			}

			if (mat.NormalTexture.IsValid())
			{
				normalTexture = mat.NormalTexture;
			}

			if (mat.SpecularTexture.IsValid())
			{
				specularTexture = mat.SpecularTexture;
			}

			// diffuse
			{
				Graphics::CombinedImageSampler ciSampler = {.ImageTexture = diffuseTexture, .ImageSampler = m_ModelSampler};
				resourceSet->WriteCombinedImageSampler(ciSampler, "diffuseMapSampler");
			}

			// normal
			{
				Graphics::CombinedImageSampler ciSampler = {.ImageTexture = normalTexture, .ImageSampler = m_ModelSampler};
				resourceSet->WriteCombinedImageSampler(ciSampler, "normalMapSampler");
			}

			// specular
			{
				Graphics::CombinedImageSampler ciSampler = {.ImageTexture = specularTexture, .ImageSampler = m_ModelSampler};
				resourceSet->WriteCombinedImageSampler(ciSampler, "specularMapSampler");
			}

			UniformBufferView modelCameraUniformView = {};
			modelCameraUniformView.BufferHandle		 = m_ModelCameraUniformBuffer;
			modelCameraUniformView.Offset			 = 0;
			modelCameraUniformView.Size				 = m_ModelCameraUniformBuffer->GetDescription().SizeInBytes;
			resourceSet->WriteUniformBuffer(modelCameraUniformView, "Camera");

			UniformBufferView modelTransformUniformView = {};
			modelTransformUniformView.BufferHandle		= transformUniformBuffer;
			modelTransformUniformView.Offset			= 0;
			modelTransformUniformView.Size				= transformUniformBuffer->GetDescription().SizeInBytes;
			resourceSet->WriteUniformBuffer(modelTransformUniformView, "Transform");

			resourceSet->Flush();

			Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
			resourceBindingDesc.TargetResourceSet							   = resourceSet;
			resourceBindingDesc.DynamicOffsets								   = {};
			m_CommandList->SetResourceSet(resourceBindingDesc);

			Ref<IDeviceBuffer> vertexBuffer		= mesh->GetVertexBuffer();
			VertexBufferView   vertexBufferView = {};
			vertexBufferView.BufferHandle		= vertexBuffer;
			vertexBufferView.Offset				= 0;
			vertexBufferView.Size				= vertexBuffer->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Ref<IDeviceBuffer> indexBuffer	   = mesh->GetIndexBuffer();
			IndexBufferView	   indexBufferView = {};
			indexBufferView.BufferHandle	   = indexBuffer;
			indexBufferView.Offset			   = 0;
			indexBufferView.BufferFormat	   = Graphics::IndexFormat::UInt32;
			indexBufferView.Size			   = indexBuffer->GetSizeInBytes();
			m_CommandList->SetIndexBuffer(indexBufferView);

			DrawIndexedDescription drawDesc = {};
			drawDesc.VertexStart			= 0;
			drawDesc.IndexStart				= 0;
			drawDesc.InstanceStart			= 0;
			drawDesc.IndexCount				= mesh->GetIndexBuffer()->GetCount();
			drawDesc.InstanceCount			= 1;
			m_CommandList->DrawIndexed(drawDesc);
		}
	}

	void Renderer3D::ClearGBuffer()
	{
		auto [width, height] = m_RenderTarget->GetSize();
		m_CommandList->Begin();
		m_CommandList->SetFramebuffer(m_RenderTarget);

		m_CommandList->SetPipeline(m_ClearScreenPipeline);

		Nexus::Graphics::Viewport vp;
		vp.X		= 0;
		vp.Y		= 0;
		vp.Width	= width;
		vp.Height	= height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_CommandList->SetViewport(vp);

		Nexus::Graphics::Scissor scissor;
		scissor.X	   = 0;
		scissor.Y	   = 0;
		scissor.Width  = width;
		scissor.Height = height;
		m_CommandList->SetScissor(scissor);

		Nexus::Graphics::VertexBufferView vertexBufferView = {};
		vertexBufferView.BufferHandle					   = m_FullscreenQuad.GetVertexBuffer();
		vertexBufferView.Offset							   = 0;
		vertexBufferView.Size							   = m_FullscreenQuad.GetVertexBuffer()->GetSizeInBytes();
		m_CommandList->SetVertexBuffer(vertexBufferView, 0);

		Nexus::Graphics::IndexBufferView indexBufferView = {};
		indexBufferView.BufferHandle					 = m_FullscreenQuad.GetIndexBuffer();
		indexBufferView.Offset							 = 0;
		indexBufferView.Size							 = m_FullscreenQuad.GetIndexBuffer()->GetSizeInBytes();
		indexBufferView.BufferFormat					 = Graphics::IndexFormat::UInt32;
		m_CommandList->SetIndexBuffer(indexBufferView);

		DrawDescription drawDesc = {};
		drawDesc.VertexStart	 = 0;
		drawDesc.InstanceStart	 = 0;
		drawDesc.VertexCount	 = 6;
		drawDesc.InstanceCount	 = 1;
		m_CommandList->Draw(drawDesc);

		m_CommandList->End();

		m_CommandQueue->SubmitCommandList(m_CommandList);
		m_CommandQueue->WaitForIdle();
	}

	void Renderer3D::CreateCubemapPipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode		 = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace		 = Nexus::Graphics::FrontFace::CounterClockwise;

		pipelineDescription.VertexModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								c_CubemapVertexShader,
																								"cubemap.vert.glsl",
																								Nexus::GetApplication()->GetApplicationPath(),
																								Nexus::Graphics::ShaderStage::Vertex);

		pipelineDescription.FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								  c_CubemapFragmentShader,
																								  "cubemap.frag.glsl",
																								  Nexus::GetApplication()->GetApplicationPath(),
																								  Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.ColourTargetCount = 2;
		pipelineDescription.ColourFormats[0]  = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourFormats[1]  = Nexus::Graphics::PixelFormat::R32_G32_UInt;
		pipelineDescription.Samples			  = 1;
		pipelineDescription.Layouts			  = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

		pipelineDescription.DepthStencilDesc.EnableDepthTest		 = false;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = false;
		pipelineDescription.DepthFormat								 = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

		pipelineDescription.ResourceDescription.Descriptors = {
			{Nexus::Graphics::ResourceDescriptor {.Name				  = "Camera",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
												  .CountOrSizeInBytes = 1},
			 Nexus::Graphics::ResourceDescriptor {.Name				  = "skybox",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
												  .CountOrSizeInBytes = 1}}};

		m_CubemapPipeline	 = m_Device->CreateGraphicsPipeline(pipelineDescription);
		m_CubemapResourceSet = m_Device->CreateResourceSet(m_CubemapPipeline);

		DeviceBufferDescription cubemapBufferDesc = {};
		cubemapBufferDesc.Access				  = Graphics::BufferMemoryAccess::Upload;
		cubemapBufferDesc.Usage					  = Graphics::BufferUsage_Uniform;
		cubemapBufferDesc.StrideInBytes			  = sizeof(CubemapCameraUniforms);
		cubemapBufferDesc.SizeInBytes			  = sizeof(CubemapCameraUniforms);
		m_CubemapUniformBuffer					  = Ref<IDeviceBuffer>(m_Device->CreateDeviceBuffer(cubemapBufferDesc));

		Nexus::Graphics::SamplerDescription samplerSpec = {};
		samplerSpec.AddressModeU						= Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						= Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						= Nexus::Graphics::SamplerAddressMode::Clamp;
		m_CubemapSampler								= m_Device->CreateSampler(samplerSpec);
	}

	void Renderer3D::CreateModelPipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode		 = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace		 = Nexus::Graphics::FrontFace::CounterClockwise;
		pipelineDescription.DepthStencilDesc.EnableDepthTest			 = true;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite			 = true;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction	 = Nexus::Graphics::ComparisonFunction::Less;

		pipelineDescription.VertexModule   = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								  c_ModelVertexShader,
																								  "model.vert.glsl",
																								  Nexus::GetApplication()->GetApplicationPath(),
																								  Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								  c_ModelFragmentShader,
																								  "model.frag.glsl",
																								  Nexus::GetApplication()->GetApplicationPath(),
																								  Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalColourTangentBitangent::GetLayout()};

		pipelineDescription.ColourTargetCount = 2;
		pipelineDescription.ColourFormats[0]  = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourFormats[1]  = Nexus::Graphics::PixelFormat::R32_G32_UInt;
		pipelineDescription.Samples			  = 1;

		pipelineDescription.ColourBlendStates[0].EnableBlending			= true;
		pipelineDescription.ColourBlendStates[0].SourceColourBlend		= Nexus::Graphics::BlendFactor::SourceAlpha;
		pipelineDescription.ColourBlendStates[0].DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDescription.ColourBlendStates[0].ColorBlendFunction		= Nexus::Graphics::BlendEquation::Add;
		pipelineDescription.ColourBlendStates[0].SourceAlphaBlend		= Nexus::Graphics::BlendFactor::One;
		pipelineDescription.ColourBlendStates[0].DestinationAlphaBlend	= Nexus::Graphics::BlendFactor::Zero;
		pipelineDescription.ColourBlendStates[0].AlphaBlendFunction		= Nexus::Graphics::BlendEquation::Add;

		pipelineDescription.ResourceDescription.Descriptors = {
			{Nexus::Graphics::ResourceDescriptor {.Name				  = "Camera",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
												  .CountOrSizeInBytes = 1},
			 Nexus::Graphics::ResourceDescriptor {.Name				  = "Transform",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::UniformBuffer,
												  .CountOrSizeInBytes = 1},
			 Nexus::Graphics::ResourceDescriptor {.Name				  = "diffuseMapSampler",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
												  .CountOrSizeInBytes = 1},
			 Nexus::Graphics::ResourceDescriptor {.Name				  = "normalMapSampler",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
												  .CountOrSizeInBytes = 1},
			 Nexus::Graphics::ResourceDescriptor {.Name				  = "specularMapSampler",
												  .Type				  = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
												  .CountOrSizeInBytes = 1}}};

		m_ModelPipeline = m_Device->CreateGraphicsPipeline(pipelineDescription);

		// model camera
		{
			DeviceBufferDescription cameraBufferDesc = {};
			cameraBufferDesc.Access					 = Graphics::BufferMemoryAccess::Upload;
			cameraBufferDesc.Usage					 = Graphics::BufferUsage_Uniform;
			cameraBufferDesc.StrideInBytes			 = sizeof(ModelCameraUniforms);
			cameraBufferDesc.SizeInBytes			 = sizeof(ModelCameraUniforms);
			m_ModelCameraUniformBuffer				 = Ref<IDeviceBuffer>(m_Device->CreateDeviceBuffer(cameraBufferDesc));
		}

		Nexus::Graphics::SamplerDescription samplerSpec = {};
		samplerSpec.AddressModeU						= Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV						= Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW						= Nexus::Graphics::SamplerAddressMode::Clamp;
		m_ModelSampler									= m_Device->CreateSampler(samplerSpec);
	}

	void Renderer3D::CreateClearGBufferPipeline()
	{
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription = {};
		pipelineDescription.RasterizerStateDesc.TriangleCullMode		 = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace		 = Nexus::Graphics::FrontFace::Clockwise;
		pipelineDescription.DepthStencilDesc.EnableDepthTest			 = true;
		pipelineDescription.DepthStencilDesc.EnableDepthWrite			 = true;
		pipelineDescription.DepthStencilDesc.DepthComparisonFunction	 = Nexus::Graphics::ComparisonFunction::Less;

		pipelineDescription.VertexModule   = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								  c_ClearGBufferVertexShader,
																								  "clearscreen.vert.glsl",
																								  Nexus::GetApplication()->GetApplicationPath(),
																								  Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								  c_ClearGBufferFragmentShader,
																								  "clearscreen.frag.glsl",
																								  Nexus::GetApplication()->GetApplicationPath(),
																								  Nexus::Graphics::ShaderStage::Fragment);

		pipelineDescription.Layouts = {m_FullscreenQuad.GetVertexBufferLayout()};

		pipelineDescription.ColourTargetCount = 2;
		pipelineDescription.ColourFormats[0]  = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourFormats[1]  = Nexus::Graphics::PixelFormat::R32_G32_UInt;
		pipelineDescription.Samples			  = 1;

		pipelineDescription.ColourBlendStates[0].EnableBlending			= true;
		pipelineDescription.ColourBlendStates[0].SourceColourBlend		= Nexus::Graphics::BlendFactor::SourceAlpha;
		pipelineDescription.ColourBlendStates[0].DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
		pipelineDescription.ColourBlendStates[0].ColorBlendFunction		= Nexus::Graphics::BlendEquation::Add;
		pipelineDescription.ColourBlendStates[0].SourceAlphaBlend		= Nexus::Graphics::BlendFactor::One;
		pipelineDescription.ColourBlendStates[0].DestinationAlphaBlend	= Nexus::Graphics::BlendFactor::Zero;
		pipelineDescription.ColourBlendStates[0].AlphaBlendFunction		= Nexus::Graphics::BlendEquation::Add;

		pipelineDescription.ColourBlendStates[1].EnableBlending = false;

		m_ClearScreenPipeline = m_Device->CreateGraphicsPipeline(pipelineDescription);
	}

}	 // namespace Nexus::Graphics