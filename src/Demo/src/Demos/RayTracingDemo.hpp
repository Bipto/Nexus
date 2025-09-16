#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

namespace Demos
{
	class RayTracingDemo : public Demo
	{
	  public:
		RayTracingDemo(const std::string						 &name,
					   Nexus::Application						 *app,
					   Nexus::ImGuiUtils::ImGuiGraphicsRenderer	 *imGuiRenderer,
					   Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
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
				vertexBufferDesc.Usage									  = Nexus::Graphics::BufferUsage::AccelerationStructureBuildInputReadOnly;
				vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
				vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
				m_VertexBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc));
				m_VertexBuffer->SetData(vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

				std::vector<uint32_t> indices = {0, 1, 2};

				Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
				indexBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
				indexBufferDesc.Usage									 = Nexus::Graphics::BufferUsage::AccelerationStructureBuildInputReadOnly;
				indexBufferDesc.StrideInBytes							 = sizeof(uint32_t);
				indexBufferDesc.SizeInBytes								 = indices.size() * sizeof(uint32_t);
				m_IndexBuffer = Nexus::Ref<Nexus::Graphics::DeviceBuffer>(m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc));
				m_IndexBuffer->SetData(indices.data(), 0, indices.size() * sizeof(uint32_t));
			}

			Nexus::Graphics::AccelerationStructureTriangleGeometry triangleDesc = {};
			triangleDesc.VertexBuffer		= Nexus::Graphics::DeviceBufferAddress {.Buffer = m_VertexBuffer, .Offset = 0};
			triangleDesc.VertexBufferFormat = Nexus::Graphics::VertexFormat::R32G32B32_SFloat;
			triangleDesc.VertexBufferStride = sizeof(Nexus::Graphics::VertexPosition);
			triangleDesc.VertexCount		= 3;
			triangleDesc.IndexBuffer		= Nexus::Graphics::DeviceBufferAddress {.Buffer = m_IndexBuffer, .Offset = 0};
			triangleDesc.IndexBufferFormat	= Nexus::Graphics::IndexFormat::UInt32;
			triangleDesc.TransformBuffer	= {};

			Nexus::Graphics::AccelerationStructureGeometryDescription geometryDesc = {};
			geometryDesc.Type													   = Nexus::Graphics::GeometryType::Triangles;
			geometryDesc.Flags													   = 0;
			geometryDesc.Geometry												   = triangleDesc;

			Nexus::Graphics::AccelerationStructureGeometryBuildDescription geometryBuildDesc = {};
			geometryBuildDesc.Type			  = Nexus::Graphics::AccelerationStructureType::BottomLevel;
			geometryBuildDesc.Flags			  = 0;
			geometryBuildDesc.Geometry		  = {geometryDesc};
			geometryBuildDesc.PrimitiveCounts = {1};
			geometryBuildDesc.Mode			  = Nexus::Graphics::AccelerationStructureBuildMode::Build;
			geometryBuildDesc.Source		  = nullptr;
			geometryBuildDesc.Destination	  = nullptr;
			geometryBuildDesc.ScratchBuffer	  = {};

			std::vector<uint32_t> primitiveCounts = {1};

			Nexus::Graphics::AccelerationStructureBuildSizeDescription buildSize =
				m_GraphicsDevice->GetAccelerationStructureBuildSize(geometryBuildDesc, primitiveCounts);

			Nexus::Graphics::DeviceBufferDescription scratchBufferDesc = {};
			scratchBufferDesc.Access								   = Nexus::Graphics::BufferMemoryAccess::Default;
			scratchBufferDesc.DebugName								   = "Scratch Buffer";
			scratchBufferDesc.SizeInBytes							   = buildSize.BuildScratchSize;
			scratchBufferDesc.StrideInBytes							   = buildSize.BuildScratchSize;
			scratchBufferDesc.Usage									   = Nexus::Graphics::BufferUsage::Storage;
			Nexus::Ref<Nexus::Graphics::DeviceBuffer> scratchBuffer	   = m_GraphicsDevice->CreateDeviceBuffer(scratchBufferDesc);

			Nexus::Graphics::DeviceBufferDescription accelerationBufferDesc = {};
			accelerationBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Default;
			accelerationBufferDesc.DebugName								= "Acceleration Structure Buffer";
			accelerationBufferDesc.SizeInBytes								= buildSize.AccelerationStructureSize;
			accelerationBufferDesc.StrideInBytes							= 0;
			accelerationBufferDesc.Usage									= Nexus::Graphics::BufferUsage::AccelerationStructureStorage;
			m_AccelerationStructureBuffer									= m_GraphicsDevice->CreateDeviceBuffer(accelerationBufferDesc);

			Nexus::Graphics::AccelerationStructureDescription accelerationStructureDesc = {};
			accelerationStructureDesc.Size												= buildSize.AccelerationStructureSize;
			accelerationStructureDesc.Type												= Nexus::Graphics::AccelerationStructureType::BottomLevel;
			accelerationStructureDesc.DebugName											= "Acceleration Structure";
			accelerationStructureDesc.Buffer											= m_AccelerationStructureBuffer;
			accelerationStructureDesc.Offset											= 0;
			m_AccelerationStructure = m_GraphicsDevice->CreateAccelerationStructure(accelerationStructureDesc);

			geometryBuildDesc.Source		= nullptr;
			geometryBuildDesc.Destination	= m_AccelerationStructure;
			geometryBuildDesc.ScratchBuffer = Nexus::Graphics::DeviceBufferAddress {.Buffer = scratchBuffer, .Offset = 0};

			Nexus::Graphics::AccelerationStructureBuildDescription accelerationStructureBuildDesc = {};
			accelerationStructureBuildDesc.Geometry												  = geometryBuildDesc;
			accelerationStructureBuildDesc.BuildRange											  = {
				Nexus::Graphics::AccelerationStructureBuildRange {.PrimitiveCount = 1, .PrimitiveOffset = 0, .FirstVertex = 0, .TransformOffset = 0}};

			m_CommandList->Begin();
			m_CommandList->BuildAccelerationStructures({accelerationStructureBuildDesc});
			m_CommandList->End();

			m_CommandQueue->SubmitCommandList(m_CommandList);

			m_GraphicsDevice->WaitForIdle();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			NX_PROFILE_FUNCTION();

			{
				NX_PROFILE_SCOPE("Command recording");

				m_CommandList->Begin();
				m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});
				m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
				m_CommandList->End();
			}

			{
				NX_PROFILE_SCOPE("Command submission");
				m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
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
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> m_AccelerationStructureBuffer;

		Nexus::Ref<Nexus::Graphics::IAccelerationStructure> m_AccelerationStructure = nullptr;
	};	  // namespace Demos
}	 // namespace Demos