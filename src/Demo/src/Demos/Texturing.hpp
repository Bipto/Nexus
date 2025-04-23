#pragma once

#include "Demo.hpp"

namespace Demos
{
	class TexturingDemo : public Demo
	{
	  public:
		TexturingDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~TexturingDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			CreatePipeline();

			Nexus::Graphics::MeshFactory factory(m_GraphicsDevice);
			m_Mesh = factory.CreateSprite();

			// m_Texture = m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/brick.jpg"), false);
			m_Texture = m_GraphicsDevice->CreateTexture2D(Nexus::FileSystem::GetFilePathAbsolute("resources/demo/textures/brick.jpg"), false);

			Nexus::Graphics::SamplerSpecification samplerSpec {};
			m_Sampler = m_GraphicsDevice->CreateSampler(samplerSpec);

			m_TextureID = m_ImGuiRenderer->BindTexture(m_Texture);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
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

			m_ResourceSet->WriteCombinedImageSampler(m_Texture, m_Sampler, "texSampler");

			m_CommandList->SetResourceSet(m_ResourceSet);

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_Mesh->GetVertexBuffer().get();
			vertexBufferView.Offset							   = 0;
			vertexBufferView.Stride							   = m_Mesh->GetVertexBuffer()->GetDescription().StrideInBytes;
			vertexBufferView.Size							   = m_Mesh->GetVertexBuffer()->GetDescription().SizeInBytes;
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_Mesh->GetIndexBuffer().get();
			indexBufferView.Offset							 = 0;
			indexBufferView.Size							 = m_Mesh->GetIndexBuffer()->GetDescription().SizeInBytes;
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexBufferFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			auto indexCount = m_Mesh->GetIndexBuffer()->GetCount();
			m_CommandList->DrawIndexed(indexCount, 0, 0);
			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual void RenderUI() override
		{
			ImGui::Image(m_TextureID, {256, 256});
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::PipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.VertexModule   = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/texturing.vert.glsl",
																										Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule = m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/texturing.frag.glsl",
																										Nexus::Graphics::ShaderStage::Fragment);

			pipelineDescription.ResourceSetSpec.SampledImages = {{"texSampler", 0, 0}};

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;
			pipelineDescription.Layouts					= {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};

			m_Pipeline	  = m_GraphicsDevice->CreatePipeline(pipelineDescription);
			m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList = nullptr;
		Nexus::Ref<Nexus::Graphics::Pipeline>	 m_Pipeline	   = nullptr;
		Nexus::Ref<Nexus::Graphics::ResourceSet> m_ResourceSet = nullptr;
		Nexus::Ref<Nexus::Graphics::Mesh>		 m_Mesh		   = nullptr;
		Nexus::Ref<Nexus::Graphics::Texture2D>	 m_Texture	   = nullptr;
		Nexus::Ref<Nexus::Graphics::Sampler>	 m_Sampler	   = nullptr;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		ImTextureID m_TextureID = 0;
	};
}	 // namespace Demos