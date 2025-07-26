#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	class IAccelerationStructure;

	struct DeviceBufferAddress
	{
		Ref<DeviceBuffer> Buffer = nullptr;
		size_t			  Offset = 0;
	};

	struct AccelerationStructureAddress
	{
		Ref<IAccelerationStructure> AccelerationStructure = nullptr;
		size_t						Offset				  = 0;
	};

	enum class DeviceBufferOrHostAddressType
	{
		DeviceBuffer,
		HostAddress
	};

	enum class AccelerationStructureType
	{
		BottomLevel,
		TopLevel
	};

	enum class GeometryType
	{
		Triangles,
		AxisAlignedBoundingBoxes,
		Instance
	};

	enum class VertexFormat
	{
		R32G32_SFloat,
		R32G32B32_SFloat,
		R16G16_SFloat,
		R16G16B16A16_SFloat,
		R16G16_SNorm,
		R16G16B16A16_SNorm,
		R16G16B16A16_UNorm,
		R16G16_UNorm,
		R10G10B10A2_UNorm,
		R8G8_UNorm,
		R8G8B8A8_UNorm,
		R8G8_SNorm
	};

	struct AccelerationStructureTriangleGeometry
	{
		DeviceBufferAddress VertexBuffer;
		VertexFormat		VertexBufferFormat = VertexFormat::R32G32B32_SFloat;
		size_t				VertexBufferStride = 0;
		size_t				VertexCount		   = 0;
		DeviceBufferAddress IndexBuffer;
		IndexFormat			IndexBufferFormat = IndexFormat::UInt32;
		DeviceBufferAddress TransformBuffer;
	};

	struct AccelerationStructureAABBGeometry
	{
		size_t				AABBCount = 0;
		DeviceBufferAddress AABBs;
		size_t				Stride = 0;
	};

	struct AccelerationStructureInstanceGeometry
	{
		DeviceBufferAddress InstanceBuffer;
		size_t				Stride			= 0;
		bool				ArrayOfPointers = false;
	};

	enum AccelerationStructureGeometryFlags : uint8_t
	{
		Opaque			  = BIT(0),
		NoDuplicateAnyhit = BIT(1)
	};

	typedef std::variant<AccelerationStructureTriangleGeometry, AccelerationStructureAABBGeometry, AccelerationStructureInstanceGeometry>
		AccelerationStructureGeometry;

	struct AccelerationStructureGeometryDescription
	{
		GeometryType				  Type	= GeometryType::Triangles;
		uint8_t						  Flags = 0;
		AccelerationStructureGeometry Geometry;
	};

	enum AccelerationStructureBuildFlags
	{
		AllowUpdate		= BIT(0),
		AllowCompaction = BIT(1),
		PreferFastTrace = BIT(2),
		PreferFastBuild = BIT(3),
		MinimizeMemory	= BIT(4),
	};

	enum class AccelerationStructureBuildMode
	{
		Build,
		Update
	};

	enum class AccelerationStructureCopyMode
	{
		Clone,
		Compact,
		Serialize,
		Deserialize
	};

	struct AccelerationStructureDescription
	{
		AccelerationStructureType Type		= AccelerationStructureType::BottomLevel;
		std::string				  DebugName = "Acceleration Structure";
		Ref<DeviceBuffer>		  Buffer	= nullptr;
		size_t					  Offset	= 0;
		size_t					  Size		= 0;
	};

	struct AccelerationStructureBuildRange
	{
		uint32_t PrimitiveCount	 = 0;
		uint32_t PrimitiveOffset = 0;
		uint32_t FirstVertex	 = 0;
		uint32_t TransformOffset = 0;
	};

	struct AccelerationStructureGeometryBuildDescription
	{
		AccelerationStructureType							  Type			  = AccelerationStructureType::BottomLevel;
		uint8_t												  Flags			  = 0;
		std::vector<AccelerationStructureGeometryDescription> Geometry		  = {};
		std::vector<uint32_t>								  PrimitiveCounts = {};
		AccelerationStructureBuildMode						  Mode			  = AccelerationStructureBuildMode::Build;
		Ref<IAccelerationStructure>							  Source;
		Ref<IAccelerationStructure>							  Destination;
		DeviceBufferAddress									  ScratchBuffer;
	};

	struct AccelerationStructureBuildDescription
	{
		AccelerationStructureGeometryBuildDescription Geometry	 = {};
		std::vector<AccelerationStructureBuildRange>  BuildRange = {};
	};

	struct AccelerationStructureCopyDescription
	{
		AccelerationStructureAddress  Source;
		AccelerationStructureCopyMode Mode = AccelerationStructureCopyMode::Clone;
	};

	struct AccelerationStructureDeviceBufferCopyDescription
	{
		AccelerationStructureAddress  Source;
		DeviceBufferAddress			  Destination;
		size_t						  WriteOffset = 0;
		AccelerationStructureCopyMode Mode		  = AccelerationStructureCopyMode::Clone;
	};

	struct DeviceBufferAccelerationStructureCopyDescription
	{
		DeviceBufferAddress			  Source	  = {};
		AccelerationStructureAddress  Destination = {};
		size_t						  ReadOffset  = 0;
		AccelerationStructureCopyMode Mode		  = AccelerationStructureCopyMode::Clone;
	};

	struct AccelerationStructureBuildSizeDescription
	{
		size_t AccelerationStructureSize = 0;
		size_t UpdateScratchSize		 = 0;
		size_t BuildScratchSize			 = 0;
	};

	class IAccelerationStructure
	{
	  public:
		virtual ~IAccelerationStructure()
		{
		}

		virtual const AccelerationStructureDescription &GetDescription() const = 0;
	};

}	 // namespace Nexus::Graphics