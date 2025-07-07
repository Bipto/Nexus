#pragma once

#include "Demo.hpp"

namespace Demos
{
	class HelloTriangleIndirectIndexedDemo : public Demo
	{
	  public:
		HelloTriangleIndirectIndexedDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~HelloTriangleIndirectIndexedDemo()
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

			Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
			vertexBufferDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Upload;
			vertexBufferDesc.Usage									  = Nexus::Graphics::BufferUsage::Vertex;
			vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
			vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
			m_VertexBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc));
			m_VertexBuffer->SetData(vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

			std::vector<uint32_t> indices = {0, 1, 2};

			Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
			indexBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			indexBufferDesc.Usage									 = Nexus::Graphics::BufferUsage::Index;
			indexBufferDesc.StrideInBytes							 = sizeof(uint32_t);
			indexBufferDesc.SizeInBytes								 = indices.size() * sizeof(uint32_t);
			m_IndexBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc));
			m_IndexBuffer->SetData(indices.data(), 0, indices.size() * sizeof(uint32_t));

			Nexus::Graphics::IndirectIndexedDrawArguments args = {};
			args.FirstIndex									   = 0;
			args.FirstInstance								   = 0;
			args.VertexOffset								   = 0;
			args.IndexCount									   = m_IndexBuffer->GetCount();
			args.InstanceCount								   = 1;

			Nexus::Graphics::DeviceBufferDescription indirectBufferDesc = {};
			indirectBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Upload;
			indirectBufferDesc.Usage									= Nexus::Graphics::BufferUsage::Indirect;
			indirectBufferDesc.StrideInBytes							= sizeof(Nexus::Graphics::IndirectIndexedDrawArguments);
			indirectBufferDesc.SizeInBytes								= sizeof(Nexus::Graphics::IndirectIndexedDrawArguments);
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

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_IndexBuffer;
			indexBufferView.Offset							 = 0;
			indexBufferView.Size							 = m_IndexBuffer->GetSizeInBytes();
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexBufferFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			Nexus::Graphics::DrawIndirectIndexedDescription drawDesc = {};
			drawDesc.IndirectBuffer									 = m_IndirectBuffer;
			drawDesc.Offset											 = 0;
			drawDesc.DrawCount										 = 1;
			m_CommandList->DrawIndexedIndirect(drawDesc);

			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a triangle using a vertex buffer, an index buffer and an indirect buffer";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.ColourTargetCount		= 1;
			pipelineDescription.ColourFormats[0]		= Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.ColourTargetSampleCount = Nexus::GetApplication()->GetPrimarySwapchain()->GetSpecification().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPosition::GetLayout()};

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
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>	  m_IndexBuffer;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>	  m_IndirectBuffer;
		glm::vec3									  m_ClearColour = {0.7f, 0.2f, 0.3f};
	};
}	 // namespace Demos