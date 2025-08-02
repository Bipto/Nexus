#include "Nexus-Core/Graphics/ShaderDataType.hpp"

namespace Nexus::Graphics
{
	uint32_t GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::R8_UInt: return 1; break;
			case ShaderDataType::R8G8_UInt: return 1 * 2; break;
			case ShaderDataType::R8G8B8A8_UInt: return 1 * 4; break;
			case ShaderDataType::R8_UNorm: return 1; break;
			case ShaderDataType::R8G8_UNorm: return 1 * 2; break;
			case ShaderDataType::R8G8B8A8_UNorm: return 1 * 4; break;
			case ShaderDataType::R32_SFloat: return 4; break;
			case ShaderDataType::R32G32_SFloat: return 4 * 2; break;
			case ShaderDataType::R32G32B32_SFloat: return 4 * 3; break;
			case ShaderDataType::R32G32B32A32_SFloat: return 4 * 4; break;
			case ShaderDataType::R16_SFloat: return 2; break;
			case ShaderDataType::R16G16_SFloat: return 2 * 2; break;
			case ShaderDataType::R16G16B16A16_SFloat: return 2 * 4; break;
			case ShaderDataType::R32_SInt: return 4; break;
			case ShaderDataType::R32G32_SInt: return 4 * 2; break;
			case ShaderDataType::R32G32B32_SInt: return 4 * 3; break;
			case ShaderDataType::R32G32B32A32_SInt: return 4 * 4; break;
			case ShaderDataType::R8_SInt: return 1; break;
			case ShaderDataType::R8G8_SInt: return 1 * 2; break;
			case ShaderDataType::R8G8B8A8_SInt: return 1 * 4; break;
			case ShaderDataType::R8_SNorm: return 1; break;
			case ShaderDataType::R8G8_SNorm: return 1 * 2; break;
			case ShaderDataType::R8G8B8A8_SNorm: return 1 * 4; break;
			case ShaderDataType::R16_SInt: return 2; break;
			case ShaderDataType::R16_SNorm: return 2; break;
			case ShaderDataType::R16G16_SInt: return 2 * 2; break;
			case ShaderDataType::R16G16_SNorm: return 2 * 2; break;
			case ShaderDataType::R16G16B16A16_SInt: return 2 * 4; break;
			case ShaderDataType::R16G16B16A16_SNorm: return 2 * 4; break;
			case ShaderDataType::R32_UInt: return 4; break;
			case ShaderDataType::R32G32_UInt: return 4 * 2; break;
			case ShaderDataType::R32G32B32_UInt: return 4 * 3; break;
			case ShaderDataType::R32G32B32A32_UInt: return 4 * 4; break;
			case ShaderDataType::R16_UInt: return 2; break;
			case ShaderDataType::R16G16_UInt: return 2 * 2; break;
			case ShaderDataType::R16G16B16A16_UInt: return 2 * 4; break;
			case ShaderDataType::R16_UNorm: return 2; break;
			case ShaderDataType::R16G16_UNorm: return 2 * 2; break;
			case ShaderDataType::R16G16B16A16_UNorm: return 2 * 4; break;
			case ShaderDataType::A2B10G10R10_UInt: return 4 * 4; break;
			case ShaderDataType::A2B10G10R10_UNorm: return 4 * 4; break;
			default: return 0; break;
		}
	}
}	 // namespace Nexus::Graphics