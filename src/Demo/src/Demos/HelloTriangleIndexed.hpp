#pragma once

#include "Demo.hpp"

namespace Demos
{
	class HelloTriangleIndexedDemo : public Demo
	{
	  public:
		HelloTriangleIndexedDemo(const std::string						  &name,
								 Nexus::Application						  *app,
								 Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer,
								 Nexus::Graphics::CommandQueueHandle	   commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~HelloTriangleIndexedDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_CommandQueue->CreateCommandList();
			CreatePipeline();

			std::vector<Nexus::Graphics::VertexPosition> vertices = {
				{{-0.5f, -0.5f, 0.0f}},	   // bottom left
				{{0.0f, 0.5f, 0.0f}},	   // top left
				{{0.5f, -0.5f, 0.0f}},	   // bottom right
			};

			Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
			vertexBufferDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Upload;
			vertexBufferDesc.Usage									  = Nexus::Graphics::BufferUsage_Vertex;
			vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
			vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
			m_VertexBuffer = Nexus::Ref<Nexus::Graphics::IDeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc));
			m_VertexBuffer->SetData(vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

			std::vector<uint32_t> indices = {0, 1, 2};

			Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
			indexBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
			indexBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_Index;
			indexBufferDesc.StrideInBytes							 = sizeof(uint32_t);
			indexBufferDesc.SizeInBytes								 = indices.size() * sizeof(uint32_t);
			m_IndexBuffer = Nexus::Ref<Nexus::Graphics::IDeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc));
			m_IndexBuffer->SetData(indices.data(), 0, indices.size() * sizeof(uint32_t));
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			auto [width, height] = m_Window->GetWindowSize();

			m_CommandList->Begin();
			Nexus::Graphics::ScopedDebugGroup debugGroup("Rendering Triangle", m_CommandList);
			m_CommandList->SetPipeline(m_Pipeline);

			Nexus::Ref<Nexus::Graphics::ISwapchain> swapchain	= Nexus::GetApplication()->GetPrimarySwapchain();
			Nexus::Graphics::FramebufferHandle		framebuffer = swapchain->GetCurrentFramebuffer();
			m_CommandList->SetFramebuffer(framebuffer);

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= width;
			vp.Height	= height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			m_CommandList->SetViewport(vp);

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = width;
			scissor.Height = height;
			m_CommandList->SetScissor(scissor);

			m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

			Nexus::Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle					   = m_VertexBuffer;
			vertexBufferView.Offset							   = 0;
			vertexBufferView.Size							   = m_VertexBuffer->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Nexus::Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle					 = m_IndexBuffer;
			indexBufferView.Offset							 = 0;
			indexBufferView.Size							 = m_IndexBuffer->GetSizeInBytes();
			indexBufferView.BufferFormat					 = Nexus::Graphics::IndexFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			Nexus::Graphics::DrawIndexedDescription drawDesc = {};
			drawDesc.VertexStart							 = 0;
			drawDesc.IndexStart								 = 0;
			drawDesc.InstanceStart							 = 0;
			drawDesc.IndexCount								 = m_IndexBuffer->GetCount();
			drawDesc.InstanceCount							 = 1;
			m_CommandList->DrawIndexed(drawDesc);

			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering a triangle using a vertex buffer and an index buffer";
		}

	  private:
		void CreatePipeline()
		{
			Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
			pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
			pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

			pipelineDescription.ColourTargetCount = 1;
			pipelineDescription.ColourFormats[0]  = Nexus::GetApplication()->GetPrimarySwapchain()->GetColourFormat();
			pipelineDescription.Samples			  = Nexus::GetApplication()->GetPrimarySwapchain()->GetDescription().Samples;

			pipelineDescription.Layouts = {Nexus::Graphics::VertexPosition::GetLayout()};

			pipelineDescription.VertexModule =
				Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																   "resources/demo/shaders/hello_triangle/hello_triangle.vert.glsl",
																   Nexus::GetApplication()->GetApplicationPath(),
																   Nexus::Graphics::ShaderStage::Vertex);
			pipelineDescription.FragmentModule =
				Nexus::Utils::GetOrCreateCachedShaderFromSpirvFile(m_GraphicsDevice,
																   "resources/demo/shaders/hello_triangle/hello_triangle.frag.glsl",
																   Nexus::GetApplication()->GetApplicationPath(),
																   Nexus::Graphics::ShaderStage::Fragment);

			m_Pipeline = m_GraphicsDevice->CreateGraphicsPipeline(pipelineDescription);
		}

	  private:
		Nexus::Ref<Nexus::Graphics::ICommandList>  m_CommandList  = {};
		Nexus::Graphics::PipelineHandle			   m_Pipeline	  = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_VertexBuffer = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_IndexBuffer  = {};
		glm::vec3								   m_ClearColour  = {0.7f, 0.2f, 0.3f};
	};
}	 // namespace Demos