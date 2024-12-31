#pragma once

#include "Demo.hpp"

namespace Demos
{
	class HelloTriangleIndexedDemo : public Demo
	{
	  public:
		HelloTriangleIndexedDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~HelloTriangleIndexedDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			CreatePipeline();

			std::vector<Nexus::Graphics::VertexPosition> vertices = {
				{{-0.5f, -0.5f, 0.0f}},	   // bottom left
				{{0.0f, 0.5f, 0.0f}},	   // top left
				{{0.5f, -0.5f, 0.0f}},	   // bottom right
			};

			Nexus::Graphics::BufferDescription vertexBufferDesc;
			vertexBufferDesc.Size  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
			vertexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
			m_VertexBuffer		   = m_GraphicsDevice->CreateVertexBuffer(vertexBufferDesc, vertices.data());

			std::vector<unsigned int> indices = {0, 1, 2};

			Nexus::Graphics::BufferDescription indexBufferDesc;
			indexBufferDesc.Size  = indices.size() * sizeof(unsigned int);
			indexBufferDesc.Usage = Nexus::Graphics::BufferUsage::Static;
			m_IndexBuffer		  = m_GraphicsDevice->CreateIndexBuffer(indexBufferDesc, indices.data());
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

			m_CommandList->SetVertexBuffer(m_VertexBuffer, 0);
			m_CommandList->SetIndexBuffer(m_IndexBuffer);

			m_CommandList->DrawIndexed(m_IndexBuffer->GetCount(), 0, 0);
			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a triangle using a vertex buffer and an index buffer";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::PipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPosition::GetLayout()};

			pipelineDescription.VertexModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/hello_triangle.vert.glsl",
																	   Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/hello_triangle.frag.glsl",
																	   Nexus::Graphics::ShaderStage::Fragment);

			m_Pipeline = m_GraphicsDevice->CreatePipeline(pipelineDescription);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList>  m_CommandList;
		Nexus::Ref<Nexus::Graphics::Pipeline>	  m_Pipeline;
		Nexus::Ref<Nexus::Graphics::VertexBuffer> m_VertexBuffer;
		Nexus::Ref<Nexus::Graphics::IndexBuffer>  m_IndexBuffer;
		glm::vec3								  m_ClearColour = {0.7f, 0.2f, 0.3f};
	};
}	 // namespace Demos