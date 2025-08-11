#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	/// @brief An enum representing the different types that can be contained within
	/// a vertex buffer
	enum class ShaderDataType
	{
		Invalid = 0,
		R8_UInt,
		R8G8_UInt,
		R8G8B8A8_UInt,
		R8_UNorm,
		R8G8_UNorm,
		R8G8B8A8_UNorm,
		R32_SFloat,
		R32G32_SFloat,
		R32G32B32_SFloat,
		R32G32B32A32_SFloat,
		R16_SFloat,
		R16G16_SFloat,
		R16G16B16A16_SFloat,
		R32_SInt,
		R32G32_SInt,
		R32G32B32_SInt,
		R32G32B32A32_SInt,
		R8_SInt,
		R8G8_SInt,
		R8G8B8A8_SInt,
		R8_SNorm,
		R8G8_SNorm,
		R8G8B8A8_SNorm,
		R16_SInt,
		R16G16_SInt,
		R16G16B16A16_SInt,
		R16_SNorm,
		R16G16_SNorm,
		R16G16B16A16_SNorm,
		R32_UInt,
		R32G32_UInt,
		R32G32B32_UInt,
		R32G32B32A32_UInt,
		R16_UInt,
		R16G16_UInt,
		R16G16B16A16_UInt,
		R16_UNorm,
		R16G16_UNorm,
		R16G16B16A16_UNorm,
		A2B10G10R10_UInt,
		A2B10G10R10_UNorm
	};

	/// @brief A method that returns the size of a shader data type in bytes
	/// @param type An enum representing the data type to get the size of
	/// @return An unsigned 32 bit integer representing the number of bytes taken by
	/// the data type
	uint32_t GetShaderDataTypeSize(ShaderDataType type);
}	 // namespace Nexus::Graphics