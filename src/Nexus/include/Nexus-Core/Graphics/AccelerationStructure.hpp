#pragma once

#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	enum class AccelerationStructureType
	{
		BottomLevel,
		TopLevel
	};

	enum class GeometryType
	{
		Triangles,
		AxisAlignedBoundingBoxes
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

	struct AccelerationStructureTriangleGeometryDescription
	{
		Ref<DeviceBuffer> VertexBuffer		 = nullptr;
		VertexFormat	  VertexBufferFormat = VertexFormat::R32G32B32_Float;
		size_t			  VertexBufferStride = 0;
		size_t			  VertexCount		 = 0;
		Ref<DeviceBuffer> IndexBuffer		 = nullptr;
		IndexFormat		  IndexBufferFormat	 = IndexFormat::UInt32;
		Ref<DeviceBuffer> TransformBuffer	 = nullptr;
	};

	struct AccelerationStructureAABBGeometryDescription
	{
		size_t			  AABBCount = 0;
		Ref<DeviceBuffer> AABBs		= nullptr;
	};

	enum AccelerationStructureGeometryFlags : uint8_t
	{
		Opaque			  = BIT(0),
		NoDuplicateAnyhit = BIT(1)
	};

	struct AccelerationStructureGeometryDescription
	{
		GeometryType																								 Type  = GeometryType::Triangles;
		uint8_t																										 Flags = 0;
		std::variant<AccelerationStructureTriangleGeometryDescription, AccelerationStructureAABBGeometryDescription> Data;
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