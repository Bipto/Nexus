#pragma once

#include "Demo.hpp"

namespace Demos
{
	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_CAMERA
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};

	struct alignas(16) VB_UNIFORM_TRANSFORM_DEMO_CAMERA
	{
		glm::mat4 Transform;
	};

	class CameraDemo : public Demo
	{
	  public:
		CameraDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer),
			  m_Camera(m_GraphicsDevice)
		{
		}

		virtual ~CameraDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
			m_Mesh = factory.CreateCube();

			m_Texture = m_GraphicsDevice->CreateTexture2D(
				Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/raw_plank_wall_diff_1k.jpg").c_str(),
				true);

			Nexus::Graphics::SamplerSpecification samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

			Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
			cameraUniformBufferDesc.Type									 = Nexus::Graphics::DeviceBufferType::Uniform;
			cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_CAMERA_DEMO_CAMERA);
			cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_CAMERA_DEMO_CAMERA);
			cameraUniformBufferDesc.HostVisible								 = true;
			m_CameraUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(cameraUniformBufferDesc));

			Nexus::Graphics::DeviceBufferDescription transformUniformBufferDesc = {};
			transformUniformBufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Uniform;
			transformUniformBufferDesc.StrideInBytes							= sizeof(VB_UNIFORM_TRANSFORM_DEMO_CAMERA);
			transformUniformBufferDesc.SizeInBytes								= sizeof(VB_UNIFORM_TRANSFORM_DEMO_CAMERA);
			transformUniformBufferDesc.HostVisible								= true;
			m_TransformUniformBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(transformUniformBufferDesc));

			CreatePipeline();
			m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.5f));
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			// upload transform
			{
				m_TransformUniforms.Transform = glm::mat4(1.0f);
				m_TransformUniformBuffer->SetData(&m_TransformUniforms, 0, sizeof(m_TransformUniforms));
			}

			// upload camera
			{
				m_CameraUniforms.View		= m_Camera.GetView();
				m_CameraUniforms.Projection = m_Camera.GetProjection();
				m_CameraUniformBuffer->SetData(&m_CameraUniforms, 0, sizeof(m_CameraUniforms));
			}

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

			m_ResourceSet->WriteCombinedImageSampler(m_Texture, m_Sampler, "texSampler");
			m_CommandList->SetResourceSet(m_ResourceSet);

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_Mesh->GetVertexBuffer().get();
			vertexBufferView.Offset							   = 0;
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_Mesh->GetIndexBuffer().get();
			indexBufferView.Offset							 = 0;
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexBufferFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			auto indexCount = m_Mesh->GetIndexBuffer()->GetCount();
			m_CommandList->DrawIndexed(indexCount, 1, 0, 0, 0);
			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandList(m_CommandList);
			m_Camera.Update(m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y, time);
		}

		virtual void Update(Nexus::TimeSpan time) override
		{
		}

		virtual std::string GetInfo() const override
		{
			return "3D scene with a cube\nControls: Right click in the scene to activate the camera, use escape to return the mouse cursor.\nUse "
				   "WASD to move the camera.";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::Clockwise;

			pipelineDescription.VertexModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/3d.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/3d.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}, {"Transform", 0, 1}};

			pipelineDescription.ResourceSetSpec.SampledImages = {{"texSampler", 1, 0}};

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;
			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();

			m_Pipeline	  = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::Mesh>		 m_Mesh;
		Nexus::Ref<Nexus::Graphics::Texture2D>	 m_Texture;
		Nexus::Ref<Nexus::Graphics::Sampler>	 m_Sampler;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;

		VB_UNIFORM_CAMERA_DEMO_CAMERA			   m_CameraUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_CameraUniformBuffer;

		VB_UNIFORM_TRANSFORM_DEMO_CAMERA		   m_TransformUniforms;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_TransformUniformBuffer;

		Nexus::FirstPersonCamera m_Camera;
	};
}	 // namespace Demos