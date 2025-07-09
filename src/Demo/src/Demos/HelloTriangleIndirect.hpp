#pragma once

#include "Demo.hpp"

namespace Demos
{
	class HelloTriangleIndirectDemo : public Demo
	{
	  public:
		HelloTriangleIndirectDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~HelloTriangleIndirectDemo()
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
			vertexBufferDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Upload;
			vertexBufferDesc.Usage									  = Nexus::Graphics::BufferUsage::Vertex;
			vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
			vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
			m_VertexBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc));
			m_VertexBuffer->SetData(vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

			CreatePipeline();

			Nexus::Graphics::IndirectDrawArguments args = {};
			args.FirstInstance							= 0;
			args.FirstVertex							= 0;
			args.VertexCount							= m_VertexBuffer->GetCount();
			args.InstanceCount							= 1;

			Nexus::Graphics::DeviceBufferDescription indirectBufferDesc = {};
			indirectBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Upload;
			indirectBufferDesc.Usage									= Nexus::Graphics::BufferUsage::Indirect;
			indirectBufferDesc.StrideInBytes							= sizeof(Nexus::Graphics::IndirectDrawArguments);
			indirectBufferDesc.SizeInBytes								= sizeof(Nexus::Graphics::IndirectDrawArguments);
			m_IndirectBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(indirectBufferDesc));
			m_IndirectBuffer->SetData(&args, 0, sizeof(args));
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
			vertexBufferView.BufferHandle					   = m_VertexBuffer;
			vertexBufferView.Offset							   = 0;
			vertexBufferView.Size							   = m_VertexBuffer->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::DrawIndirectDescription drawDesc = {};
			drawDesc.IndirectBuffer							  = m_IndirectBuffer;
			drawDesc.Offset									  = 0;
			drawDesc.DrawCount								  = 1;
			drawDesc.Stride									  = m_IndirectBuffer->GetStrideInBytes();
			m_CommandList->DrawIndirect(drawDesc);

			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual void OnResize(Nexus::Point2D<uint32_t> size) override
		{
		}

		virtual void RenderUI() override
		{
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a triangle using a vertex buffer and an indirect buffer";
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
		Nexus::Ref<Nexus::Graphics::CommandList>	  m_CommandList;
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> m_Pipeline;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>	  m_VertexBuffer;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>	  m_IndirectBuffer;
		glm::vec3									  m_ClearColour = {0.7f, 0.2f, 0.3f};
	};
}	 // namespace Demos