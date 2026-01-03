#pragma once

#include "DeviceBuffer.hpp"
#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	class IAccelerationStructure;

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
		DeviceAddress			   VertexBuffer		  = {};
		VertexFormat			   VertexBufferFormat = VertexFormat::R32G32B32_SFloat;
		size_t					   VertexBufferStride = 0;
		size_t					   VertexCount		  = 0;
		DeviceAddress			   IndexBuffer		  = {};
		std::optional<IndexFormat> IndexBufferFormat  = {};
		DeviceAddress			   TransformBuffer	  = {};
		size_t					   IndexCount		  = 0;
	};

	struct AccelerationStructureAABBGeometry
	{
		size_t		  Count	 = 0;
		DeviceAddress AABBs	 = {};
		size_t		  Stride = 0;
	};

	enum AccelerationStructureGeometryInstanceFlags : uint8_t
	{
		NoFlags					  = 0,
		TriangleFacingCullDisable = 0x00000001,
		TriangleFlipFacing		  = 0x00000002,
		ForceOpaque				  = 0x00000004,
		ForceNoOpaque			  = 0x00000008
	};

	struct TransformMatrix
	{
		float matrix[3][4];
	};

	struct AccelerationStructureInstance
	{
		TransformMatrix Transform									= {};
		uint32_t		InstanceCustomIndex : 24					= 0;
		uint32_t		Mask : 8									= 0;
		uint32_t		InstanceShaderBindingTableRecordOffset : 24 = 0;
		uint32_t		Flags : 8									= AccelerationStructureGeometryInstanceFlags::NoFlags;
		DeviceAddress	AccelerationStructureReference				= 0;
	};

	static_assert(sizeof(AccelerationStructureInstance) == 64, "AccelerationStructureInstance size must be 64 bytes");

	struct AccelerationStructureInstanceGeometry
	{
		DeviceAddress InstanceBuffer  = {};
		size_t		  Stride		  = 0;
		bool		  ArrayOfPointers = false;
		size_t		  Count			  = 0;
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
		GeometryType				  Type	   = GeometryType::Triangles;
		uint8_t						  Flags	   = 0;
		AccelerationStructureGeometry Geometry = {};
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
		Ref<IDeviceBuffer>		  Buffer	= nullptr;
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
		Ref<IAccelerationStructure>							  Source		  = {};
		Ref<IAccelerationStructure>							  Destination	  = {};
		DeviceAddress										  ScratchBuffer	  = {};
	};

	struct AccelerationStructureCopyDescription
	{
		DeviceAddress				  Source = {};
		AccelerationStructureCopyMode Mode	 = AccelerationStructureCopyMode::Clone;
	};

	struct AccelerationStructureDeviceBufferCopyDescription
	{
		DeviceAddress				  Source	  = {};
		DeviceAddress				  Destination = {};
		size_t						  WriteOffset = 0;
		AccelerationStructureCopyMode Mode		  = AccelerationStructureCopyMode::Clone;
	};

	struct DeviceBufferAccelerationStructureCopyDescription
	{
		DeviceAddress				  Source	  = {};
		DeviceAddress				  Destination = {};
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

		virtual const AccelerationStructureDescription &GetDescription() const				  = 0;
		virtual DeviceAddress							GetDeviceAddress(size_t offset) const = 0;
	};

}	 // namespace Nexus::Graphics