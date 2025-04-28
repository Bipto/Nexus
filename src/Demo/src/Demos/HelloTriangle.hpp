#pragma once

#include "Demo.hpp"

namespace Demos
{
	class HelloTriangleDemo : public Demo
	{
	  public:
		HelloTriangleDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~HelloTriangleDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			std::vector<Nexus::Graphics::VertexPosition> vertices = {
				{{-0.5f, -0.5f, 0.0f}},	   // bottom left
				{{0.0f, 0.5f, 0.0f}},	   // top left
				{{0.5f, -0.5f, 0.0f}},	   // bottom right
			};

			Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
			vertexBufferDesc.Type									  = Nexus::Graphics::DeviceBufferType::Vertex;
			vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
			vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
			vertexBufferDesc.HostVisible							  = true;
			m_VertexBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc));
			m_VertexBuffer->SetData(vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

			CreatePipeline();
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

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_VertexBuffer.get();
			vertexBufferView.Offset							   = 0;
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			auto vertexCount = m_VertexBuffer->GetCount();
			m_CommandList->Draw(vertexCount, 1, 0, 0);
			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual void OnResize(Nexus::Point2D<uint32_t> size) override
		{
		}

		virtual void RenderUI() override
		{
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a triangle using a vertex buffer";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
			pipelineDescription.Layouts								  = {Nexus::Graphics::VertexPosition::GetLayout()};

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;

			pipelineDescription.VertexModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/hello_triangle.vert.glsl",
																	   Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				m_GraphicsDevice->GetOrCreateCachedShaderFromSpirvFile("resources/demo/shaders/hello_triangle.frag.glsl",
																	   Nexus::Graphics::ShaderStage::Fragment);

			m_Pipeline = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList>  m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_VertexBuffer;
		glm::vec3								  m_ClearColour = {0.7f, 0.2f, 0.3f};
	};
}	 // namespace Demos