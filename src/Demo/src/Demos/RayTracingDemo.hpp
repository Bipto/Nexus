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

			Nexus::Graphics::AccelerationStructureProperties accelerationStructureProperties = m_GraphicsDevice->GetAccelerationStructureProperties();

			// set up buffers
			{
				std::vector<Nexus::Graphics::VertexPosition> vertices = {
					{{-0.5f, -0.5f, 0.0f}},	   // bottom left
					{{0.0f, 0.5f, 0.0f}},	   // top left
					{{0.5f, -0.5f, 0.0f}},	   // bottom right
				};

				Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
				vertexBufferDesc.Access									  = Nexus::Graphics::BufferMemoryAccess::Default;
				vertexBufferDesc.Usage									  = Nexus::Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly;
				vertexBufferDesc.StrideInBytes							  = sizeof(Nexus::Graphics::VertexPosition);
				vertexBufferDesc.SizeInBytes							  = vertices.size() * sizeof(Nexus::Graphics::VertexPosition);
				vertexBufferDesc.DebugName								  = "Acceleration Structure Vertex Buffer";
				m_VertexBuffer											  = m_GraphicsDevice->CreateDeviceBuffer(vertexBufferDesc);
				m_CommandQueue->WriteToBuffer(m_VertexBuffer, vertices.data(), 0, vertices.size() * sizeof(Nexus::Graphics::VertexPosition));

				std::vector<uint32_t> indices = {0, 1, 2};

				Nexus::Graphics::DeviceBufferDescription indexBufferDesc = {};
				indexBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Default;
				indexBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly;
				indexBufferDesc.StrideInBytes							 = sizeof(uint32_t);
				indexBufferDesc.SizeInBytes								 = indices.size() * sizeof(uint32_t);
				indexBufferDesc.DebugName								 = "Acceleration Structure Index Buffer";
				m_IndexBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(indexBufferDesc);
				m_CommandQueue->WriteToBuffer(m_IndexBuffer, indices.data(), 0, indices.size() * sizeof(uint32_t));
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

				std::vector<uint32_t> primitiveCounts = {1};

				Nexus::Graphics::AccelerationStructureGeometryBuildDescription geometryBuildDesc = {};
				geometryBuildDesc.Type			  = Nexus::Graphics::AccelerationStructureType::BottomLevel;
				geometryBuildDesc.Flags			  = 0;
				geometryBuildDesc.Geometry		  = {geometryDesc};
				geometryBuildDesc.PrimitiveCounts = primitiveCounts;
				geometryBuildDesc.Mode			  = Nexus::Graphics::AccelerationStructureBuildMode::Build;
				geometryBuildDesc.Source		  = nullptr;
				geometryBuildDesc.Destination	  = nullptr;
				geometryBuildDesc.ScratchBuffer	  = {};

				Nexus::Graphics::AccelerationStructureBuildSizeDescription buildSize =
					m_GraphicsDevice->GetAccelerationStructureBuildSize(geometryBuildDesc, primitiveCounts);

				size_t scratchBufferSize =
					buildSize.BuildScratchSize + (accelerationStructureProperties.MinAccelerationStructureScratchOffsetAlignment - 1);

				Nexus::Graphics::DeviceBufferDescription scratchBufferDesc = {};
				scratchBufferDesc.Access								   = Nexus::Graphics::BufferMemoryAccess::Default;
				scratchBufferDesc.DebugName								   = "Scratch Buffer";
				scratchBufferDesc.SizeInBytes							   = scratchBufferSize;
				scratchBufferDesc.StrideInBytes							   = scratchBufferSize;
				scratchBufferDesc.Usage									   = Nexus::Graphics::BufferUsage_Storage;
				Nexus::Ref<Nexus::Graphics::IDeviceBuffer> scratchBuffer   = m_GraphicsDevice->CreateDeviceBuffer(scratchBufferDesc);

				Nexus::Graphics::DeviceBufferDescription accelerationBufferDesc = {};
				accelerationBufferDesc.Access									= Nexus::Graphics::BufferMemoryAccess::Default;
				accelerationBufferDesc.DebugName								= "BLAS Buffer";
				accelerationBufferDesc.SizeInBytes								= buildSize.AccelerationStructureSize;
				accelerationBufferDesc.StrideInBytes							= buildSize.AccelerationStructureSize;
				accelerationBufferDesc.Usage									= Nexus::Graphics::BufferUsage_AccelerationStructureStorage;
				m_BLASBuffer													= m_GraphicsDevice->CreateDeviceBuffer(accelerationBufferDesc);

				Nexus::Graphics::AccelerationStructureDescription accelerationStructureDesc = {};
				accelerationStructureDesc.Size												= buildSize.AccelerationStructureSize;
				accelerationStructureDesc.Type												= Nexus::Graphics::AccelerationStructureType::BottomLevel;
				accelerationStructureDesc.DebugName											= "BLAS";
				accelerationStructureDesc.Buffer											= m_BLASBuffer;
				accelerationStructureDesc.Offset											= 0;
				m_BLAS = m_GraphicsDevice->CreateAccelerationStructure(accelerationStructureDesc);

				geometryBuildDesc.Source	  = nullptr;
				geometryBuildDesc.Destination = m_BLAS;

				const uint64_t align			= accelerationStructureProperties.MinAccelerationStructureScratchOffsetAlignment;
				const uint64_t baseAddr			= scratchBuffer->GetDeviceAddress(0);
				const uint64_t alignedAddr		= (baseAddr + (align - 1)) & ~(align - 1);
				const uint64_t alignedOffset	= alignedAddr - baseAddr;
				geometryBuildDesc.ScratchBuffer = alignedAddr;

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
				instance.Transform.matrix[0][0]							= 1.f;
				instance.Transform.matrix[0][1]							= 0.f;
				instance.Transform.matrix[0][2]							= 0.f;
				instance.Transform.matrix[0][3]							= 0.f;
				instance.Transform.matrix[1][0]							= 0.f;
				instance.Transform.matrix[1][1]							= 1.f;
				instance.Transform.matrix[1][2]							= 0.f;
				instance.Transform.matrix[1][3]							= 0.f;
				instance.Transform.matrix[2][0]							= 0.f;
				instance.Transform.matrix[2][1]							= 0.f;
				instance.Transform.matrix[2][2]							= 1.f;
				instance.Transform.matrix[2][3]							= 0.f;

				instance.Flags							= Nexus::Graphics::AccelerationStructureGeometryInstanceFlags::ForceOpaque;
				instance.AccelerationStructureReference = m_BLAS->GetDeviceAddress(0);

				Nexus::Graphics::DeviceBufferDescription transformBufferDesc = {};
				transformBufferDesc.Access									 = Nexus::Graphics::BufferMemoryAccess::Default;
				transformBufferDesc.Usage									 = Nexus::Graphics::BufferUsage_AccelerationStructureBuildInputReadOnly;
				transformBufferDesc.StrideInBytes							 = sizeof(Nexus::Graphics::AccelerationStructureInstance);
				transformBufferDesc.SizeInBytes								 = sizeof(Nexus::Graphics::AccelerationStructureInstance);
				m_TransformBuffer											 = m_GraphicsDevice->CreateDeviceBuffer(transformBufferDesc);
				m_CommandQueue->WriteToBuffer(m_TransformBuffer, &instance, 0, sizeof(Nexus::Graphics::AccelerationStructureInstance));

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

				size_t scratchBufferSize =
					buildSize.BuildScratchSize + (accelerationStructureProperties.MinAccelerationStructureScratchOffsetAlignment - 1);

				Nexus::Graphics::DeviceBufferDescription scratchBufferDesc = {};
				scratchBufferDesc.Access								   = Nexus::Graphics::BufferMemoryAccess::Default;
				scratchBufferDesc.DebugName								   = "Scratch Buffer";
				scratchBufferDesc.SizeInBytes							   = scratchBufferSize;
				scratchBufferDesc.StrideInBytes							   = scratchBufferSize;
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
				accelerationStructureDesc.Buffer											= m_TLASBuffer;
				accelerationStructureDesc.Offset											= 0;
				m_TLAS = m_GraphicsDevice->CreateAccelerationStructure(accelerationStructureDesc);

				geometryBuildDesc.Source	  = nullptr;
				geometryBuildDesc.Destination = m_TLAS;

				const uint64_t align			= accelerationStructureProperties.MinAccelerationStructureScratchOffsetAlignment;
				const uint64_t baseAddr			= scratchBuffer->GetDeviceAddress(0);
				const uint64_t alignedAddr		= (baseAddr + (align - 1)) & ~(align - 1);
				const uint64_t alignedOffset	= alignedAddr - baseAddr;
				geometryBuildDesc.ScratchBuffer = alignedAddr;

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

			// storage texture
			{
				Nexus::Graphics::TextureDescription textureDesc = {};
				textureDesc.Width								= 500;
				textureDesc.Height								= 500;
				textureDesc.DepthOrArrayLayers					= 1;
				textureDesc.DebugName							= "Ray Tracing Output Image";
				textureDesc.Usage								= Nexus::Graphics::TextureUsage_Storage | Nexus::Graphics::TextureUsage_Sampled;
				m_StorageTexture								= m_GraphicsDevice->CreateTexture(textureDesc);

				Nexus::Graphics::TextureViewDescription textureViewDesc = {};
				textureViewDesc.Format									= m_StorageTexture->GetPixelFormat();
				textureViewDesc.Range									= {.BaseMipLevel = 0, .LevelCount = 1, .BaseArrayLayer = 0, .LayerCount = 1};
				textureViewDesc.TargetTexture							= m_StorageTexture;
				textureViewDesc.DebugName								= "Ray Tracing Output Image View";
				m_StorageTextureView									= m_GraphicsDevice->CreateTextureView(textureViewDesc);

				m_BoundImGuiTextureID = m_ImGuiRenderer->BindTexture(m_StorageTextureView);
			}

			//// Pipeline
			{
				Nexus::Graphics::RayTracingPipelineDescription pipelineDesc = {};
				pipelineDesc.Shaders.push_back(m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/demo/shaders/ray_tracing/raygen.rgen",
																								 Nexus::Graphics::ShaderStage::RayGeneration));
				pipelineDesc.Shaders.push_back(m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/demo/shaders/ray_tracing/miss.rmiss",
																								 Nexus::Graphics::ShaderStage::RayMiss));
				pipelineDesc.Shaders.push_back(
					m_GraphicsDevice->CreateShaderModuleFromSpirvFile("resources/demo/shaders/ray_tracing/closesthit.rchit",
																	  Nexus::Graphics::ShaderStage::RayClosestHit));

				Nexus::Graphics::ShaderGroup rayGenGroup = {};
				rayGenGroup.Type						 = Nexus::Graphics::ShaderGroupType::General;
				rayGenGroup.GeneralShader				 = 0;
				rayGenGroup.ClosestHitShader			 = NX_SHADER_UNUSED;
				rayGenGroup.AnyHitShader				 = NX_SHADER_UNUSED;
				rayGenGroup.IntersectionShader			 = NX_SHADER_UNUSED;

				Nexus::Graphics::ShaderGroup missGroup = {};
				missGroup.Type						   = Nexus::Graphics::ShaderGroupType::General;
				missGroup.GeneralShader				   = 1;
				missGroup.ClosestHitShader			   = NX_SHADER_UNUSED;
				missGroup.AnyHitShader				   = NX_SHADER_UNUSED;
				missGroup.IntersectionShader		   = NX_SHADER_UNUSED;

				Nexus::Graphics::ShaderGroup hitGroup = {};
				hitGroup.Type						  = Nexus::Graphics::ShaderGroupType::Triangles;
				hitGroup.GeneralShader				  = NX_SHADER_UNUSED;
				hitGroup.ClosestHitShader			  = 2;
				hitGroup.AnyHitShader				  = NX_SHADER_UNUSED;
				hitGroup.IntersectionShader			  = NX_SHADER_UNUSED;

				pipelineDesc.ShaderGroups = {rayGenGroup, missGroup, hitGroup};

				pipelineDesc.MaxRecursionDepth				 = 1;
				pipelineDesc.DebugName						 = "Ray Tracing Pipeline";
				pipelineDesc.ResourceDescription.Descriptors = {
					Nexus::Graphics::ResourceDescriptor {.Name				 = "outputImage",
														 .Type				 = Nexus::Graphics::ResourceDescriptorType::StorageImage,
														 .CountOrSizeInBytes = 1},
					Nexus::Graphics::ResourceDescriptor {.Name				 = "topLevelAS",
														 .Type				 = Nexus::Graphics::ResourceDescriptorType::AccelerationStructure,
														 .CountOrSizeInBytes = 1}};

				m_Pipeline = m_GraphicsDevice->CreateRayTracingPipeline(pipelineDesc);

				m_ResourceSet = m_GraphicsDevice->CreateResourceSet(m_Pipeline);
				m_ResourceSet->WriteAccelerationStructure(m_TLAS, "topLevelAS");

				Nexus::Graphics::StorageImageView storageImageView = {};
				storageImageView.TextureHandle					   = m_StorageTexture;
				storageImageView.Access							   = Nexus::Graphics::ShaderAccess::ReadWrite;
				storageImageView.ArrayLayer						   = 0;
				storageImageView.MipLevel						   = 0;
				m_ResourceSet->WriteStorageImage(storageImageView, "outputImage");

				m_ResourceSet->Flush();
			}

			// SBT
			{
				Nexus::Graphics::RayTracingDeviceDescription deviceRayTracingDesc = m_GraphicsDevice->GetRayTracingDeviceDescription();
				uint32_t									 handleSize			  = deviceRayTracingDesc.ShaderGroupHandleSize;
				uint32_t									 handleAlignment	  = deviceRayTracingDesc.ShaderGroupBaseAlignment;

				std::vector<uint8_t> handles = m_Pipeline->GetRayTracingShaderGroupHandles();

				Nexus::Graphics::DeviceBufferDescription sbtDesc = {};
				// group count is 3: raygen, miss, hit. This is multiplied by handle alignment to ensure proper spacing
				sbtDesc.SizeInBytes	  = 3 * handleAlignment;
				sbtDesc.StrideInBytes = handleSize;
				sbtDesc.Access		  = Nexus::Graphics::BufferMemoryAccess::Default;
				sbtDesc.Flags		  = Nexus::Graphics::BufferCreateFlags_None;
				sbtDesc.Usage		  = Nexus::Graphics::BufferUsage::BufferUsage_ShaderBindingTable;
				sbtDesc.DebugName	  = "SBT";
				m_SBT				  = m_GraphicsDevice->CreateDeviceBuffer(sbtDesc);

				// ray gen
				const size_t raygenIndex = 0;
				m_CommandQueue->WriteToBuffer(m_SBT, handles.data() + (handleSize * raygenIndex), (handleAlignment * raygenIndex), handleAlignment);

				// miss
				const size_t missIndex = (1);
				m_CommandQueue->WriteToBuffer(m_SBT, handles.data() + (handleSize * missIndex), (handleAlignment * missIndex), handleAlignment);

				// closest hit
				const size_t hitIndex = (2);
				m_CommandQueue->WriteToBuffer(m_SBT, handles.data() + (handleSize * hitIndex), (handleAlignment * hitIndex), handleAlignment);

				m_RaygenRegion = {.Address = m_SBT->GetDeviceAddress(handleAlignment * raygenIndex),
								  .Stride  = handleAlignment,
								  .Size	   = handleAlignment};
				m_MissRegion = {.Address = m_SBT->GetDeviceAddress(handleAlignment * missIndex), .Stride = handleAlignment, .Size = handleAlignment};
				m_HitRegion	 = {.Address = m_SBT->GetDeviceAddress(handleAlignment * hitIndex), .Stride = handleAlignment, .Size = handleAlignment};
				m_CallableRegion = {.Address = 0, .Stride = 0, .Size = 0};
			}
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

				m_CommandQueue->SubmitCommandList(m_CommandList);
				m_GraphicsDevice->WaitForIdle();
			}

			{
				NX_PROFILE_SCOPE("Command submission");

				m_CommandList->Begin();
				m_CommandList->SetPipeline(m_Pipeline);
				m_CommandList->SetResourceSet({.TargetResourceSet = m_ResourceSet, .DynamicOffsets = {}});

				Nexus::Graphics::TraceRaysDescription traceRaysDesc = {};
				traceRaysDesc.RaygenRegion							= m_RaygenRegion;
				traceRaysDesc.MissRegion							= m_MissRegion;
				traceRaysDesc.HitRegion								= m_HitRegion;
				traceRaysDesc.CallableRegion						= m_CallableRegion;
				traceRaysDesc.Width									= m_StorageTexture->GetWidth();
				traceRaysDesc.Height								= m_StorageTexture->GetHeight();
				traceRaysDesc.Depth									= 1;
				m_CommandList->TraceRays(traceRaysDesc);

				m_CommandList->End();

				m_CommandQueue->SubmitCommandList(m_CommandList);
				m_GraphicsDevice->WaitForIdle();
			}
		}

		virtual void RenderUI() override
		{
			ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColour));
			ImGui::Image(m_BoundImGuiTextureID, ImVec2(500, 500));
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

		Nexus::Ref<Nexus::Graphics::IRayTracingPipeline> m_Pipeline	   = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer>		 m_SBT		   = nullptr;
		Nexus::Ref<Nexus::Graphics::IResourceSet>		 m_ResourceSet = nullptr;

		Nexus::Graphics::StridedDeviceAddressRegion m_RaygenRegion	 = {};
		Nexus::Graphics::StridedDeviceAddressRegion m_MissRegion	 = {};
		Nexus::Graphics::StridedDeviceAddressRegion m_HitRegion		 = {};
		Nexus::Graphics::StridedDeviceAddressRegion m_CallableRegion = {};

		Nexus::Ref<Nexus::Graphics::ITexture>	  m_StorageTexture		= nullptr;
		Nexus::Ref<Nexus::Graphics::ITextureView> m_StorageTextureView	= nullptr;
		ImTextureID								  m_BoundImGuiTextureID = 0;

	};	  // namespace Demos
}	 // namespace Demos