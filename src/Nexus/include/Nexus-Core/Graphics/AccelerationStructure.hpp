#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	struct DeviceBufferAddress
	{
		Ref<DeviceBuffer> Buffer = nullptr;
		size_t			  Offset = 0;
	};

	enum class DeviceBufferOrHostAddressType
	{
		DeviceBuffer,
		HostAddress
	};

	typedef const void									  *HostAddress;
	typedef std::variant<DeviceBufferAddress, HostAddress> DeviceBufferOrHostAddress;

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
		DeviceBufferOrHostAddress VertexBuffer;
		VertexFormat			  VertexBufferFormat = VertexFormat::R32G32B32_SFloat;
		size_t					  VertexBufferStride = 0;
		size_t					  VertexCount		 = 0;
		DeviceBufferOrHostAddress IndexBuffer;
		IndexFormat				  IndexBufferFormat = IndexFormat::UInt32;
		DeviceBufferOrHostAddress TransformBuffer;
	};

	struct AccelerationStructureAABBGeometry
	{
		size_t					  AABBCount = 0;
		DeviceBufferOrHostAddress AABBs;
		size_t					  Stride = 0;
	};

	struct AccelerationStructureInstanceGeometry
	{
		DeviceBufferOrHostAddress InstanceBuffer;
		size_t					  Stride		  = 0;
		bool					  ArrayOfPointers = false;
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

	struct AccelerationStructureBuildDescription
	{
		AccelerationStructureType							  Type			  = AccelerationStructureType::BottomLevel;
		uint8_t												  Flags			  = 0;
		std::vector<AccelerationStructureGeometryDescription> Geometry		  = {};
		std::vector<uint32_t>								  PrimitiveCounts = {};
		AccelerationStructureBuildMode						  Mode			  = AccelerationStructureBuildMode::Build;
	};

	struct AccelerationStructureBuildSizeDescription
	{
		size_t AccelerationStructureSize = 0;
		size_t UpdateScratchSize		 = 0;
		size_t BuildScratchSize			 = 0;
	};
}	 // namespace Nexus::Graphics