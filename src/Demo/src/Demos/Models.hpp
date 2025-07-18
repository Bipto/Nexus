#pragma once

#include "Demo.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace Demos
{
	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_MODELS
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 CamPosition;
	};

	struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_MODELS
	{
		glm::mat4 Transform;
	};

	class ModelDemo : public Demo
	{
	  public:
		ModelDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer),
			  m_Camera(m_GraphicsDevice)
		{
		}

		virtual ~ModelDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
			m_Model = factory.CreateFrom3DModelFile(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/models/The Boss/The Boss.dae"));

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			cameraUniformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage::Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
			m_CameraUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc));

			Nexus::Graphics::DeviceBufferDescription transformUniformBufferDesc = {};
			transformUniformBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Upload;
			transformUniformBufferDesc.Usage									= Nexus::Graphics::BufferUsage::Uniform;
			transformUniformBufferDesc.StrideInBytes							= sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
			transformUniformBufferDesc.SizeInBytes								= sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
			m_TransformUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc));

			CreatePipeline();
			m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

			Nexus::Graphics::SamplerDescription samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CameraUniforms.View		 = m_Camera.GetView();
			m_CameraUniforms.Projection	 = m_Camera.GetProjection();
			m_CameraUniforms.CamPosition = m_Camera.GetPosition();
			m_CameraUniformBuffer->SetData(&m_CameraUniforms, 0, sizeof(m_CameraUniforms));

			m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), {0.0f, 0. - 1.0f, 5.0f});
			m_TransformUniformBuffer->SetData(&m_TransformUniforms, 0, sizeof(m_TransformUniforms));

			m_CommandList->Begin();
			m_CommandList->SetPipeline(m_Pipeline);
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(Nexus::GetApplication()->GetPrimarySwapchain()));

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().X;
			vp.Height	= Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().Y;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			m_CommandList->SetViewport(vp);

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().X;
			scissor.Height = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().Y;
			m_CommandList->SetScissor(scissor);

			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

			Nexus::Graphics::ClearDepthStencilValue value;
			m_CommandList->ClearDepthTarget(value);

			const auto &meshes = m_Model->GetMeshes();

			for (size_t i = 0; i < meshes.size(); i++)
			{
				const auto &mesh		= meshes[i];
				const auto &mat			= mesh->GetMaterial();
				const auto	resourceSet = m_ResourceSets[i];

				Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
				cameraUniformBufferView.BufferHandle					   = m_CameraUniformBuffer;
				cameraUniformBufferView.Offset							   = 0;
				cameraUniformBufferView.Size							   = m_CameraUniformBuffer->GetDescription().SizeInBytes;
				resourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

				Nexus::Graphics::UniformBufferView transformUniformBufferView = {};
				transformUniformBufferView.BufferHandle						  = m_TransformUniformBuffer;
				transformUniformBufferView.Offset							  = 0;
				transformUniformBufferView.Size								  = m_TransformUniformBuffer->GetDescription().SizeInBytes;
				resourceSet->WriteUniformBuffer(transformUniformBufferView, "Transform");

				if (mat.DiffuseTexture)
				{
					resourceSet->WriteCombinedImageSampler(mat.DiffuseTexture, m_Sampler, "diffuseMapSampler");
				}
				m_CommandList->SetResourceSet(resourceSet);

				Nexus::Graphics::VertexBufferView vertexBufferView = {};
				vertexBufferView.BufferHandle					   = mesh->GetVertexBuffer();
				vertexBufferView.Offset							   = 0;
				vertexBufferView.Size							   = mesh->GetVertexBuffer()->GetSizeInBytes();
				m_CommandList->SetVertexBuffer(vertexBufferView, 0);

				Nexus::Graphics::IndexBufferView indexBufferView = {};
				indexBufferView.BufferHandle					 = mesh->GetIndexBuffer();
				indexBufferView.Offset							 = 0;
				indexBufferView.Size							 = mesh->GetIndexBuffer()->GetSizeInBytes();
				indexBufferView.BufferFormat					 = Nexus::Graphics::IndexBufferFormat::UInt32;
				m_CommandList->SetIndexBuffer(indexBufferView);

				auto indexCount = mesh->GetIndexBuffer()->GetCount();

				Nexus::Graphics::DrawIndexedDescription drawDesc = {};
				drawDesc.VertexStart							 = 0;
				drawDesc.IndexStart								 = 0;
				drawDesc.InstanceStart							 = 0;
				drawDesc.IndexCount								 = indexCount;
				drawDesc.InstanceCount							 = 1;
				m_CommandList->DrawIndexed(drawDesc);
			}

			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();

			m_Rotation += 0.05f * time.GetMilliseconds<float>();
		}

		virtual void Update(Nexus::TimeSpan time) override
		{
			m_Camera.Update(m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y, time);
		}

		virtual std::string GetInfo() const override
		{
			return "Loading and rendering a 3D model.";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode	 = Nexus::Graphics::CullMode::CullNone;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace	 = Nexus::Graphics::FrontFace::Clockwise;
			pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
			pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
			pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

			pipelineDescription.VertexModule   = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/models.vert.glsl",
																										Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/models.frag.glsl",
																										Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalColourTangentBitangent::GetLayout()};

			pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}, {"Transform", 0, 1}};

			pipelineDescription.ResourceSetSpec.SampledImages = {{"diffuseMapSampler", 1, 0}};

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

			m_Pipeline = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);

			for (size_t i = 0; i < m_Model->GetMeshes().size(); i++)
			{
				Nexus::Ref<Nexus::Graphics::ResourceSet> resourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
				m_ResourceSets.push_back(resourceSet);
			}
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::Model>		 m_Model;
		glm::vec3								 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		std::vector<Nexus::Ref<Nexus::Graphics::ResourceSet>> m_ResourceSets;

		VB_UNIFORM_CAMERA_DEMO_MODELS			   m_CameraUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_CameraUniformBuffer;

		VB_UNIFORM_TRANSFORM_DEMO_MODELS		   m_TransformUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_TransformUniformBuffer;

		Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler;

		Nexus::FirstPersonCamera m_Camera;

		float m_Rotation = 0.0f;
	};
}	 // namespace Demos