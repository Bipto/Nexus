#pragma once

#include "Demo.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace Demos
{
	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_LIGHTING
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 CamPosition;
	};

	struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_LIGHTING
	{
		glm::mat4 Transform;
	};

	class LightingDemo : public Demo
	{
	  public:
		LightingDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer),
			  m_Camera(m_GraphicsDevice)
		{
		}

		virtual ~LightingDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
			m_CubeMesh = factory.CreateCube();

			m_DiffuseMap =
				m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_diff_1k.jpg"), true);
			m_NormalMap =
				m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_normal_1k.jpg"),
												  true);
			m_SpecularMap =
				m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_spec_1k.jpg"), true);

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Type									 = Nexus::Graphics::DeviceBufferType::Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_LIGHTING);
			cameraUniformBufferDesc.HostVisible								 = true;
			m_CameraUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc));

			Nexus::Graphics::DeviceBufferDescription transformUniformBufferDesc = {};
			transformUniformBufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Uniform;
			transformUniformBufferDesc.StrideInBytes							= sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
			transformUniformBufferDesc.SizeInBytes								= sizeof(VB_UNIFORM_TRANSFORM_DEMO_LIGHTING);
			transformUniformBufferDesc.HostVisible								= true;
			m_TransformUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc));

			CreatePipeline();
			m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.5f));

			Nexus::Graphics::SamplerSpecification samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_TransformUniforms.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0)) *
											glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), {1.0f, 1.0f, 0.0f});
			m_TransformUniformBuffer->SetData(&m_TransformUniforms, 0, sizeof(m_TransformUniforms));

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
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			Nexus::Graphics::ClearDepthStencilValue clearValue;
			m_CommandList->ClearDepthTarget(clearValue);

			// upload resources
			{
				Nexus::Graphics::UniformBufferView cameraUniformBufferView = {};
				cameraUniformBufferView.BufferHandle					   = m_CameraUniformBuffer.get();
				cameraUniformBufferView.Offset							   = 0;
				cameraUniformBufferView.Size							   = m_CameraUniformBuffer->GetDescription().SizeInBytes;
				m_ResourceSet->WriteUniformBuffer(cameraUniformBufferView, "Camera");

				Nexus::Graphics::UniformBufferView transformUniformBufferView = {};
				transformUniformBufferView.BufferHandle						  = m_TransformUniformBuffer.get();
				transformUniformBufferView.Offset							  = 0;
				transformUniformBufferView.Size								  = m_TransformUniformBuffer->GetDescription().SizeInBytes;
				m_ResourceSet->WriteUniformBuffer(transformUniformBufferView, "Transform");

				m_ResourceSet->WriteCombinedImageSampler(m_DiffuseMap, m_Sampler, "diffuseMapSampler");
				m_ResourceSet->WriteCombinedImageSampler(m_NormalMap, m_Sampler, "normalMapSampler");
				m_ResourceSet->WriteCombinedImageSampler(m_SpecularMap, m_Sampler, "specularMapSampler");

				m_CommandList->SetResourceSet(m_ResourceSet);
			}

			// draw cube
			{
				Nexus::Graphics::VertexBufferView vertexBufferView = {};
				vertexBufferView.BufferHandle					   = m_CubeMesh->GetVertexBuffer().get();
				vertexBufferView.Offset							   = 0;
				m_CommandList->SetVertexBuffer(vertexBufferView, 0);

				Nexus::Graphics::IndexBufferView indexBufferView = {};
				indexBufferView.BufferHandle					 = m_CubeMesh->GetIndexBuffer().get();
				indexBufferView.Offset							 = 0;
				indexBufferView.BufferFormat					 = Nexus::Graphics::IndexBufferFormat::UInt32;
				m_CommandList->SetIndexBuffer(indexBufferView);

				auto indexCount = m_CubeMesh->GetIndexBuffer()->GetCount();
				m_CommandList->DrawIndexed(indexCount, 1, 0, 0, 0);
			}

			m_CommandList->SetPipeline(m_Pipeline);

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
			return "Rendering a cube using basic Blinn-Phong lighting.";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

			pipelineDescription.VertexModule   = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/lighting.vert.glsl",
																										Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/lighting.frag.glsl",
																										Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}, {"Transform", 0, 1}};

			pipelineDescription.ResourceSetSpec.SampledImages = {{"diffuseMapSampler", 1, 0},
																 {"normalMapSampler", 1, 1},
																 {"specularMapSampler", 1, 2}};

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::Mesh>		 m_CubeMesh;

		Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;
		Nexus::Ref<Nexus::Graphics::Texture>	 m_DiffuseMap;
		Nexus::Ref<Nexus::Graphics::Texture>	 m_NormalMap;
		Nexus::Ref<Nexus::Graphics::Texture>	 m_SpecularMap;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		VB_UNIFORM_CAMERA_DEMO_LIGHTING			   m_CameraUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_CameraUniformBuffer;

		VB_UNIFORM_TRANSFORM_DEMO_LIGHTING		   m_TransformUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_TransformUniformBuffer;

		Nexus::Ref<Nexus::Graphics::Sampler> m_Sampler;

		Nexus::FirstPersonCamera m_Camera;

		float m_Rotation = 0.0f;
	};
}	 // namespace Demos