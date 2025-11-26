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
			m_CommandList = m_CommandQueue->CreateCommandList();

			// set up buffers
			{
				std::vector<Nexus::Graphics::VertexPosition> vertices = {
					{{-0.5f, -0.5f, 0.0f}},	   // bottom left
					{{0.0f, 0.5f, 0.0f}},	   // top left
					{{0.5f, -0.5f, 0.0f}},	   // bottom right
				};

				Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
				vertexBufferDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Upload;
				vertexBufferDesc.Usage									  = Nexus::Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly;
				vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
				vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
				m_VertexBuffer											  = m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc);
				m_VertexBuffer->SetData(vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

				std::vector<uint32_t> indices = {0, 1, 2};

				Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
				indexBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
				indexBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly;
				indexBufferDesc.StrideInBytes							 = sizeof(uint32_t);
				indexBufferDesc.SizeInBytes								 = indices.size() * sizeof(uint32_t);
				m_IndexBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc);
				m_IndexBuffer->SetData(indices.data(), 0, indices.size() * sizeof(uint32_t));
			}

			// BLAS
			{
				Nexus::Graphics::AccelerationStructureTriangleGeometry triangleDesc = {};
				triangleDesc.VertexBuffer											= m_VertexBuffer->GetDeviceAddress(0);
				triangleDesc.VertexBufferFormat										= Nexus::Graphics::VertexFormat::R32G32B32_SFloat;
				triangleDesc.VertexBufferStride										= sizeof(Nexus::Graphics::VertexPosition);
				triangleDesc.VertexCount											= 3;
				triangleDesc.IndexBuffer											= m_IndexBuffer->GetDeviceAddress(0);
				triangleDesc.IndexBufferFormat										= Nexus::Graphics::IndexFormat::UInt32;
				triangleDesc.TransformBuffer										= {};

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
				scratchBufferDesc.Usage									   = Nexus::Graphics::BufferUsage_Storage;
				Nexus::Ref<Nexus::Graphics::IDeviceBuffer> scratchBuffer   = m_GraphicsDevice->CreateDeviceBuffer(scratchBufferDesc);

				Nexus::Graphics::DeviceBufferDescription accelerationBufferDesc = {};
				accelerationBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Default;
				accelerationBufferDesc.DebugName								= "BLAS Buffer";
				accelerationBufferDesc.SizeInBytes								= buildSize.AccelerationStructureSize;
				accelerationBufferDesc.StrideInBytes							= 0;
				accelerationBufferDesc.Usage									= Nexus::Graphics::BufferUsage_AccelerationStructureStorage;
				m_BLASBuffer													= m_GraphicsDevice->CreateDeviceBuffer(accelerationBufferDesc);

				Nexus::Graphics::AccelerationStructureDescription accelerationStructureDesc = {};
				accelerationStructureDesc.Size												= buildSize.AccelerationStructureSize;
				accelerationStructureDesc.Type												= Nexus::Graphics::AccelerationStructureType::BottomLevel;
				accelerationStructureDesc.DebugName											= "BLAS";
				accelerationStructureDesc.Buffer											= m_BLASBuffer;
				accelerationStructureDesc.Offset											= 0;
				m_BLAS = m_GraphicsDevice->CreateAccelerationStructure(accelerationStructureDesc);

				geometryBuildDesc.Source		= nullptr;
				geometryBuildDesc.Destination	= m_BLAS;
				geometryBuildDesc.ScratchBuffer = scratchBuffer->GetDeviceAddress(0);

				Nexus::Graphics::AccelerationStructureBuildDescription accelerationStructureBuildDesc = {};
				accelerationStructureBuildDesc.Geometry												  = geometryBuildDesc;
				accelerationStructureBuildDesc.BuildRange = {Nexus::Graphics::AccelerationStructureBuildRange {.PrimitiveCount	= 1,
																											   .PrimitiveOffset = 0,
																											   .FirstVertex		= 0,
																											   .TransformOffset = 0}};

				m_CommandList->Begin();
				m_CommandList->BuildAccelerationStructures({accelerationStructureBuildDesc});
				m_CommandList->End();

				m_CommandQueue->SubmitCommandList(m_CommandList);

				m_GraphicsDevice->WaitForIdle();
			}

			// TLAS
			{
				// transform buffer
				Nexus::Graphics::AccelerationStructureInstance instance = {};
				instance.Transform										= glm::identity<glm::mat3x4>();
				instance.Flags											= Nexus::Graphics::AccelerationStructureGeometryInstanceFlags::ForceOpaque;
				instance.AccelerationStructureReference					= m_BLAS->GetDeviceAddress(0);

				Nexus::Graphics::DeviceBufferDescription transformBufferDesc = {};
				transformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Upload;
				transformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly;
				transformBufferDesc.StrideInBytes							 = sizeof(Nexus::Graphics::AccelerationStructureInstance);
				transformBufferDesc.SizeInBytes								 = sizeof(Nexus::Graphics::AccelerationStructureInstance);
				m_TransformBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(transformBufferDesc);
				m_TransformBuffer->SetData(&instance, 0, sizeof(Nexus::Graphics::AccelerationStructureInstance));

				Nexus::Graphics::AccelerationStructureInstanceGeometry instanceDesc = {};
				instanceDesc.InstanceBuffer											= m_TransformBuffer->GetDeviceAddress(0);
				instanceDesc.Stride													= m_TransformBuffer->GetStrideInBytes();
				instanceDesc.ArrayOfPointers										= false;

				Nexus::Graphics::AccelerationStructureGeometryDescription geometryDesc = {};
				geometryDesc.Type													   = Nexus::Graphics::GeometryType::Instance;
				geometryDesc.Flags													   = 0;
				geometryDesc.Geometry												   = instanceDesc;

				Nexus::Graphics::AccelerationStructureGeometryBuildDescription geometryBuildDesc = {};
				geometryBuildDesc.Type			  = Nexus::Graphics::AccelerationStructureType::TopLevel;
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
				scratchBufferDesc.Usage									   = Nexus::Graphics::BufferUsage_Storage;
				Nexus::Ref<Nexus::Graphics::IDeviceBuffer> scratchBuffer   = m_GraphicsDevice->CreateDeviceBuffer(scratchBufferDesc);

				Nexus::Graphics::DeviceBufferDescription accelerationBufferDesc = {};
				accelerationBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Default;
				accelerationBufferDesc.DebugName								= "TLAS Buffer";
				accelerationBufferDesc.SizeInBytes								= buildSize.AccelerationStructureSize;
				accelerationBufferDesc.StrideInBytes							= 0;
				accelerationBufferDesc.Usage									= Nexus::Graphics::BufferUsage_AccelerationStructureStorage;
				m_TLASBuffer													= m_GraphicsDevice->CreateDeviceBuffer(accelerationBufferDesc);

				Nexus::Graphics::AccelerationStructureDescription accelerationStructureDesc = {};
				accelerationStructureDesc.Size												= buildSize.AccelerationStructureSize;
				accelerationStructureDesc.Type												= Nexus::Graphics::AccelerationStructureType::TopLevel;
				accelerationStructureDesc.DebugName											= "TLAS";
				accelerationStructureDesc.Buffer											= m_BLASBuffer;
				accelerationStructureDesc.Offset											= 0;
				m_TLAS = m_GraphicsDevice->CreateAccelerationStructure(accelerationStructureDesc);

				geometryBuildDesc.Source		= nullptr;
				geometryBuildDesc.Destination	= m_TLAS;
				geometryBuildDesc.ScratchBuffer = scratchBuffer->GetDeviceAddress(0);

				Nexus::Graphics::AccelerationStructureBuildDescription accelerationStructureBuildDesc = {};
				accelerationStructureBuildDesc.Geometry												  = geometryBuildDesc;
				accelerationStructureBuildDesc.BuildRange = {Nexus::Graphics::AccelerationStructureBuildRange {.PrimitiveCount	= 1,
																											   .PrimitiveOffset = 0,
																											   .FirstVertex		= 0,
																											   .TransformOffset = 0}};

				m_CommandList->Begin();
				m_CommandList->BuildAccelerationStructures({accelerationStructureBuildDesc});
				m_CommandList->End();

				m_CommandQueue->SubmitCommandList(m_CommandList);

				m_GraphicsDevice->WaitForIdle();
			}

			// Pipeline
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			NX_PROFILE_FUNCTION();

			{
				NX_PROFILE_SCOPE("Command recording");

				m_CommandList->Begin();
				Nexus::Ref<Nexus::Graphics::ISwapchain>	  swapchain	  = Nexus::GetApplication()->GetPrimarySwapchain();
				Nexus::Ref<Nexus::Graphics::IFramebuffer> framebuffer = swapchain->GetCurrentFramebuffer();
				m_CommandList->SetFramebuffer(framebuffer);
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
		Nexus::Ref<Nexus::Graphics::ICommandList> m_CommandList;
		glm::vec3								  m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_VertexBuffer	 = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_IndexBuffer	 = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_TransformBuffer = nullptr;

		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>			m_BLASBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::IAccelerationStructure> m_BLAS		 = nullptr;

		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>			m_TLASBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::IAccelerationStructure> m_TLAS		 = nullptr;

	};	  // namespace Demos
}	 // namespace Demos