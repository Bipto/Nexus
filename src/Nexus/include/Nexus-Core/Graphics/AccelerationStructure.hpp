#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	enum class DeviceBufferOrHostAddressType
	{
		DeviceBuffer,
		HostAddress
	};

	typedef const void									*HostAddress;
	typedef std::variant<Ref<DeviceBuffer>, HostAddress> DeviceBufferOrHostAddress;

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
		R32G32_Float,
		R32G32B32_Float,
		R16G16_Float,
		R16G16B16A16_Float,
		R16G16_Snorm,
		R16G16B16A16_Snorm,
		R16G16B16A16_Unorm,
		R16G16_Unorm,
		R10G10B10A2_Unorm,
		R8G8_Unorm,
		R8G8B8A8_Unorm,
		R8G8_Snorm
	};

	struct AccelerationStructureTriangleGeometry
	{
		DeviceBufferOrHostAddress VertexBuffer;
		VertexFormat			  VertexBufferFormat = VertexFormat::R32G32B32_Float;
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
		size_t					  Stride = 0;
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
		AccelerationStructureType							  Type	   = AccelerationStructureType::BottomLevel;
		uint8_t												  Flags	   = 0;
		std::vector<AccelerationStructureGeometryDescription> Geometry = {};
		AccelerationStructureBuildMode						  Mode	   = AccelerationStructureBuildMode::Build;
	};

	struct AccelerationStructureBuildSizeDescription
	{
		size_t AccelerationStructureSize = 0;
		size_t UpdateScratchSize		 = 0;
		size_t BuildScratchSize			 = 0;
	};
}	 // namespace Nexus::Graphics