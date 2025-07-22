#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

namespace Demos
{
	class RayTracingDemo : public Demo
	{
	  public:
		RayTracingDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~RayTracingDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			// set up buffers
			{
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
			}

			Nexus::Graphics::AccelerationStructureTriangleGeometryDescription triangleDesc = {};
			triangleDesc.VertexBuffer													   = m_VertexBuffer;
			triangleDesc.VertexBufferFormat												   = Nexus::Graphics::VertexFormat::R32G32B32_Float;
			triangleDesc.VertexBufferStride												   = sizeof(Nexus::Graphics::VertexPosition);
			triangleDesc.VertexCount													   = 3;
			triangleDesc.IndexBuffer													   = m_IndexBuffer;
			triangleDesc.IndexBufferFormat												   = Nexus::Graphics::IndexFormat::UInt32;
			triangleDesc.TransformBuffer												   = nullptr;

			Nexus::Graphics::AccelerationStructureGeometryDescription geometryDesc = {};
			geometryDesc.Type													   = Nexus::Graphics::GeometryType::Triangles;
			geometryDesc.Flags													   = 0;
			geometryDesc.Data													   = triangleDesc;

			Nexus::Graphics::AccelerationStructureBuildDescription buildDesc = {};
			buildDesc.Type													 = Nexus::Graphics::AccelerationStructureType::BottomLevel;
			buildDesc.Flags													 = 0;
			buildDesc.Geometry												 = {geometryDesc};
			buildDesc.Mode													 = Nexus::Graphics::AccelerationStructureBuildMode::Build;

			Nexus::Graphics::AccelerationStructureBuildSizeDescription buildSize = m_GraphicsDevice->GetAccelerationStructureBuildSize(buildDesc, 1);
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			NX_PROFILE_FUNCTION();

			{
				NX_PROFILE_SCOPE("Command recording");

				Nexus::IWindow *window		= Nexus::GetApplication()->GetPrimaryWindow();
				int32_t			clearWidth	= window->GetWindowSize().X / 2;
				int32_t			clearHeight = window->GetWindowSize().Y / 2;

				m_CommandList->Begin();
				m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});

				Nexus::Graphics::ClearRect clearRect = {};
				clearRect.X							 = 0;
				clearRect.Y							 = 0;
				clearRect.Width						 = clearWidth;
				clearRect.Height					 = clearHeight;
				m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

				m_CommandList->End();
			}

			{
				NX_PROFILE_SCOPE("Command submission");
				m_GraphicsDevice->SubmitCommandLists(&m_CommandList, 1, nullptr);
				m_GraphicsDevice->WaitForIdle();
			}
		}

		virtual void RenderUI() override
		{
			ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColour));
		}

		virtual std::string GetInfo() const override
		{
			return "Clearing the screen using a pickable colour";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_VertexBuffer;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_IndexBuffer;
	};
}	 // namespace Demos