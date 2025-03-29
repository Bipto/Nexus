#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Graphics/HdriProcessor.hpp"
#include "stb_image.h"

namespace Demos
{

	struct alignas(16) VB_UNIFORM_CAMERA_DEMO_CUBEMAP
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};

	class CubemapDemo : public Demo
	{
	  public:
		CubemapDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer),
			  m_Camera(m_GraphicsDevice)
		{
		}

		virtual ~CubemapDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			CreatePipeline();

			Nexus::Graphics::BufferDescription cameraUniformBufferDesc;
			cameraUniformBufferDesc.Size  = sizeof(VB_UNIFORM_CAMERA_DEMO_CUBEMAP);
			cameraUniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
			m_CameraUniformBuffer		  = m_GraphicsDevice->CreateUniformBuffer(cameraUniformBufferDesc, nullptr);

			m_Camera.SetPosition(glm::vec3(0, 0, 0));

			Nexus::Graphics::SamplerSpecification samplerSpec {};
			samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Clamp;
			samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Clamp;
			samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Clamp;
			m_Sampler				 = m_GraphicsDevice->CreateSampler(samplerSpec);

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
			m_Cube = factory.CreateCube();

			Nexus::Graphics::HdriProcessor processor(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/hdri/hangar_interior_4k.hdr"),
													 m_GraphicsDevice);
			m_Cubemap = processor.Generate(2048);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CameraUniforms.Projection = m_Camera.GetProjection();
			m_CameraUniforms.View		= glm::mat4(glm::mat3(m_Camera.GetView()));
			m_CameraUniformBuffer->SetData(&m_CameraUniforms, sizeof(m_CameraUniforms));

			m_CommandList->Begin();
			m_CommandList->SetPipeline(m_Pipeline);
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});

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

			Nexus::Graphics::ClearDepthStencilValue clearDepth {};
			m_CommandList->ClearDepthTarget(clearDepth);

			m_ResourceSet->WriteUniformBuffer(m_CameraUniformBuffer, "Camera");
			m_ResourceSet->WriteCombinedImageSampler(m_Cubemap, m_Sampler, "skybox");
			m_CommandList->SetResourceSet(m_ResourceSet);

			m_CommandList->SetVertexBuffer(m_Cube->GetVertexBuffer(), 0);
			m_CommandList->SetIndexBuffer(m_Cube->GetIndexBuffer());

			auto indexCount = m_Cube->GetIndexBuffer()->GetDescription().Size / sizeof(unsigned int);
			m_CommandList->DrawIndexed(indexCount, 0, 0);

			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual void Update(Nexus::TimeSpan time) override
		{
			m_Camera.Update(m_Window->GetWindowSize().X, m_Window->GetWindowSize().Y, time);
		}

		virtual void RenderUI() override
		{
		}

		void CreatePipeline()
		{
			Nexus::Graphics::PipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.VertexModule   = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/cubemap.vert.glsl",
																										Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/cubemap.frag.glsl",
																										Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}};

			pipelineDescription.ResourceSetSpec.SampledImages = {{"skybox", 1, 0}};

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			m_Pipeline	  = m_GraphicsDevice->CreatePipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

		virtual std::string GetInfo() const override
		{
			return "Creating and rendering a cubemap to provide an environment.";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		Nexus::Ref<Nexus::Graphics::Cubemap>	 m_Cubemap;
		Nexus::Ref<Nexus::Graphics::Sampler>	 m_Sampler;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Ref<Nexus::Graphics::Pipeline>	 m_Pipeline;
		Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet;

		Nexus::Ref<Nexus::Graphics::Mesh> m_Cube;

		VB_UNIFORM_CAMERA_DEMO_CAMERA			   m_CameraUniforms;
		Nexus::Ref<Nexus::Graphics::UniformBuffer> m_CameraUniformBuffer;

		Nexus::FirstPersonCamera m_Camera;
	};
}	 // namespace Demos