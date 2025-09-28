#pragma once

#include "Demo.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace Demos
{
	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_INSTANCING
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 CamPosition;
	};

	class InstancingDemo : public Demo
	{
	  public:
		InstancingDemo(const std::string						 &name,
					   Nexus::Application						 *app,
					   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
					   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~InstancingDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_CommandQueue->CreateCommandList();

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice, m_CommandQueue);
			m_CubeMesh = factory.CreateCube();

			m_DiffuseMap =
				m_GraphicsDevice->CreateTexture2D(m_CommandQueue,
												  Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_diff_1k.jpg"),
												  true);
			m_NormalMap =
				m_GraphicsDevice->CreateTexture2D(m_CommandQueue,
												  Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_normal_1k.jpg"),
												  true);
			m_SpecularMap =
				m_GraphicsDevice->CreateTexture2D(m_CommandQueue,
												  Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_spec_1k.jpg"),
												  true);

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			cameraUniformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage::Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_INSTANCING);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_INSTANCING);
			m_CameraUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc));

			Nexus::Graphics::DeviceBufferDescription instanceBufferDesc = {};
			instanceBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Upload;
			instanceBufferDesc.Usage									= Nexus::Graphics::BufferUsage::Vertex;
			instanceBufferDesc.StrideInBytes							= sizeof(glm::mat4);
			instanceBufferDesc.SizeInBytes								= m_InstanceCount * sizeof(glm::mat4);
			m_InstanceBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(instanceBufferDesc));

			std::vector<glm::mat4> mvps(m_InstanceCount);
			for (uint32_t i = 0; i < m_InstanceCount; i++) { mvps[i] = glm::translate(glm::mat4(1.0f), glm::vec3(i * 2.0f, 0.0f, 2.5f)); }
			m_InstanceBuffer->SetData(mvps.data(), 0, mvps.size() * sizeof(glm::mat4));

			CreatePipeline();
			m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.5f));

			Nexus::Graphics::SamplerDescription samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_Rotation += time.GetSeconds<float>();
			m_CameraUniforms.View		 = m_Camera.GetView();
			m_CameraUniforms.Projection	 = m_Camera.GetProjection();
			m_CameraUniforms.CamPosition = m_Camera.GetPosition();
			m_CameraUniformBuffer->SetData(&m_CameraUniforms, 0, sizeof(m_CameraUniforms));

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
			m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			Nexus::Graphics::ClearDepthStencilValue clearValue;
			m_CommandList->ClearDepthTarget(clearValue);

			// upload resources
			{
				Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
				cameraUniformBufferView.BufferHandle					   = m_CameraUniformBuffer;
				cameraUniformBufferView.Offset							   = 0;
				cameraUniformBufferView.Size							   = m_CameraUniformBuffer->GetDescription().SizeInBytes;
				m_ResourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

				m_ResourceSet->WriteCombinedImageSampler(m_DiffuseMap, m_Sampler, "diffuseMapSampler");
				m_ResourceSet->WriteCombinedImageSampler(m_NormalMap, m_Sampler, "normalMapSampler");
				m_ResourceSet->WriteCombinedImageSampler(m_SpecularMap, m_Sampler, "specularMapSampler");

				m_CommandList->SetResourceSet(m_ResourceSet);
			}

			// draw cube
			{
				Nexus::Graphics::VertexBufferView vertexBufferView = {};
				vertexBufferView.BufferHandle					   = m_CubeMesh->GetVertexBuffer();
				vertexBufferView.Offset							   = 0;
				vertexBufferView.Size							   = m_CubeMesh->GetVertexBuffer()->GetSizeInBytes();
				m_CommandList->SetVertexBuffer(vertexBufferView, 0);

				Nexus::Graphics::VertexBufferView instanceBufferView = {};
				instanceBufferView.BufferHandle						 = m_InstanceBuffer;
				instanceBufferView.Offset							 = 0;
				instanceBufferView.Size								 = m_InstanceBuffer->GetSizeInBytes();
				m_CommandList->SetVertexBuffer(instanceBufferView, 1);

				Nexus::Graphics::IndexBufferView indexBufferView = {};
				indexBufferView.BufferHandle					 = m_CubeMesh->GetIndexBuffer();
				indexBufferView.Offset							 = 0;
				indexBufferView.Size							 = m_CubeMesh->GetIndexBuffer()->GetSizeInBytes();
				indexBufferView.BufferFormat					 = Nexus::Graphics::IndexFormat::UInt32;
				m_CommandList->SetIndexBuffer(indexBufferView);

				auto indexCount	   = m_CubeMesh->GetIndexBuffer()->GetCount();
				auto instanceCount = m_InstanceBuffer->GetSizeInBytes() / m_InstanceBuffer->GetStrideInBytes();

				Nexus::Graphics::DrawIndexedDescription drawDesc = {};
				drawDesc.VertexStart							 = 0;
				drawDesc.IndexStart								 = 0;
				drawDesc.InstanceStart							 = 0;
				drawDesc.IndexCount								 = indexCount;
				drawDesc.InstanceCount							 = instanceCount;
				m_CommandList->DrawIndexed(drawDesc);
			}

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();

			m_Rotation += 0.05f * time.GetMilliseconds<float>();
		}

		virtual void Update(Nexus::TimeSpan time) override
		{
			m_Camera.Update(m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y, time);
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a series of textured cubes using a vertex buffer containing vertices, an index buffer\nand a second vertex buffer "
				   "containing matrix transformations per cube.";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

			pipelineDescription.VertexModule   = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/instancing.vert.glsl",
																										Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/instancing.frag.glsl",
																										Nexus::Graphics::ShaderStage::Fragment);

			Nexus::Graphics::VertexBufferLayout vertexLayout = {{{Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
																 {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"},
																 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
																 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
																 {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"}},
																sizeof(VB_UNIFORM_CAMERA_DEMO_INSTANCING),
																Nexus::Graphics::StepRate::Vertex};

			Nexus::Graphics::VertexBufferLayout instanceLayout = {{{Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
																   {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
																   {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
																   {Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"}},
																  sizeof(glm::mat4),
																  Nexus::Graphics::StepRate::Instance};

			pipelineDescription.Layouts = {vertexLayout, instanceLayout};

			pipelineDescription.DepthStencilDesc.EnableDepthTest		 = true;
			pipelineDescription.DepthStencilDesc.EnableDepthWrite		 = true;
			pipelineDescription.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList>	  m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::Mesh>			  m_CubeMesh;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>	  m_InstanceBuffer;

		Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;
		Nexus::Ref<Nexus::Graphics::Texture>	 m_DiffuseMap;
		Nexus::Ref<Nexus::Graphics::Texture>	 m_NormalMap;
		Nexus::Ref<Nexus::Graphics::Texture>	 m_SpecularMap;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		VB_UNIFORM_CAMERA_DEMO_INSTANCING		  m_CameraUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_CameraUniformBuffer;

		Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler;

		Nexus::FirstPersonCamera m_Camera;

		const uint32_t m_InstanceCount = 10;

		float m_Rotation = 0.0f;
	};
}	 // namespace Demos