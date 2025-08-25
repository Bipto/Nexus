#include "Vk.hpp"

#include "AccelerationStructureVk.hpp"
#include "GraphicsDeviceVk.hpp"
#include "PipelineVk.hpp"
#include "ResourceSetVk.hpp"
#include "ShaderModuleVk.hpp"

#if defined(NX_PLATFORM_VULKAN)

namespace Nexus::Vk
{
	VkFormat GetVkPixelDataFormat(Nexus::Graphics::PixelFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::PixelFormat::R8_UNorm: return VK_FORMAT_R8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_SNorm: return VK_FORMAT_R8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_UInt: return VK_FORMAT_R8_UINT;
			case Nexus::Graphics::PixelFormat::R8_SInt: return VK_FORMAT_R8_SINT;

			case Nexus::Graphics::PixelFormat::R16_UNorm: return VK_FORMAT_R16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_SNorm: return VK_FORMAT_R16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_UInt: return VK_FORMAT_R16_UINT;
			case Nexus::Graphics::PixelFormat::R16_SInt: return VK_FORMAT_R16_SINT;
			case Nexus::Graphics::PixelFormat::R16_Float: return VK_FORMAT_R16_SFLOAT;

			case Nexus::Graphics::PixelFormat::R32_UInt: return VK_FORMAT_R32_UINT;
			case Nexus::Graphics::PixelFormat::R32_SInt: return VK_FORMAT_R32_SINT;
			case Nexus::Graphics::PixelFormat::R32_Float: return VK_FORMAT_R32_SFLOAT;

			case Nexus::Graphics::PixelFormat::R8_G8_UNorm: return VK_FORMAT_R8G8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_SNorm: return VK_FORMAT_R8G8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_UInt: return VK_FORMAT_R8G8_UINT;
			case Nexus::Graphics::PixelFormat::R8_G8_SInt: return VK_FORMAT_R8G8_SINT;

			case Nexus::Graphics::PixelFormat::R16_G16_UNorm: return VK_FORMAT_R16G16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_SNorm: return VK_FORMAT_R16G16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_UInt: return VK_FORMAT_R16G16_UINT;
			case Nexus::Graphics::PixelFormat::R16_G16_SInt: return VK_FORMAT_R16G16_SINT;
			case Nexus::Graphics::PixelFormat::R16_G16_Float: return VK_FORMAT_R16G16_SFLOAT;

			case Nexus::Graphics::PixelFormat::R32_G32_UInt: return VK_FORMAT_R32G32_UINT;
			case Nexus::Graphics::PixelFormat::R32_G32_SInt: return VK_FORMAT_R32G32_SINT;
			case Nexus::Graphics::PixelFormat::R32_G32_Float: return VK_FORMAT_R32G32_SFLOAT;

			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm: return VK_FORMAT_B8G8R8A8_UNORM;
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm: return VK_FORMAT_R8G8B8A8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt: return VK_FORMAT_R8G8B8A8_UINT;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt: return VK_FORMAT_R8G8B8A8_SINT;

			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm: return VK_FORMAT_R16G16B16A16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt: return VK_FORMAT_R16G16B16A16_UINT;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt: return VK_FORMAT_R16G16B16A16_SINT;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float: return VK_FORMAT_R16G16B16A16_SFLOAT;

			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt: return VK_FORMAT_R32G32B32A32_UINT;
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt: return VK_FORMAT_R32G32B32A32_SINT;
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float: return VK_FORMAT_R32G32B32A32_SFLOAT;

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A1_UNorm: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A8_UNorm: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_UNorm: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;

			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRgb: return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRgb: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
			case Nexus::Graphics::PixelFormat::BC2_UNorm: return VK_FORMAT_BC2_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC2_UNorm_SRgb: return VK_FORMAT_BC2_SRGB_BLOCK;
			case Nexus::Graphics::PixelFormat::BC3_UNorm: return VK_FORMAT_BC3_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC3_UNorm_SRgb: return VK_FORMAT_BC3_SRGB_BLOCK;
			case Nexus::Graphics::PixelFormat::BC4_UNorm: return VK_FORMAT_BC4_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC4_SNorm: return VK_FORMAT_BC4_SNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC5_UNorm: return VK_FORMAT_BC5_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC5_SNorm: return VK_FORMAT_BC5_SNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC7_UNorm: return VK_FORMAT_BC7_UNORM_BLOCK;
			case Nexus::Graphics::PixelFormat::BC7_UNorm_SRgb: return VK_FORMAT_BC7_SRGB_BLOCK;

			case Nexus::Graphics::PixelFormat::D16_UNorm: return VK_FORMAT_D16_UNORM;
			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
			case Nexus::Graphics::PixelFormat::D32_SFloat: return VK_FORMAT_D32_SFLOAT;
			case Nexus::Graphics::PixelFormat::D32_SFloat_S8_UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	Nexus::Graphics::PixelFormat GetNxPixelFormatFromVkPixelFormat(VkFormat format)
	{
		switch (format)
		{
			case VK_FORMAT_R8_UNORM: return Nexus::Graphics::PixelFormat::R8_UNorm;
			case VK_FORMAT_R8_SNORM: return Nexus::Graphics::PixelFormat::R8_SNorm;
			case VK_FORMAT_R8_UINT: return Nexus::Graphics::PixelFormat::R8_UInt;
			case VK_FORMAT_R8_SINT: return Nexus::Graphics::PixelFormat::R8_SInt;

			case VK_FORMAT_D16_UNORM:
			case VK_FORMAT_R16_UNORM: return Nexus::Graphics::PixelFormat::R16_UNorm;
			case VK_FORMAT_R16_SNORM: return Nexus::Graphics::PixelFormat::R16_SNorm;
			case VK_FORMAT_R16_UINT: return Nexus::Graphics::PixelFormat::R16_UInt;
			case VK_FORMAT_R16_SINT: return Nexus::Graphics::PixelFormat::R16_SInt;
			case VK_FORMAT_R16_SFLOAT: return Nexus::Graphics::PixelFormat::R16_Float;

			case VK_FORMAT_R32_UINT: return Nexus::Graphics::PixelFormat::R32_UInt;
			case VK_FORMAT_R32_SINT: return Nexus::Graphics::PixelFormat::R32_SInt;
			case VK_FORMAT_D32_SFLOAT:
			case VK_FORMAT_R32_SFLOAT: return Nexus::Graphics::PixelFormat::R32_Float;

			case VK_FORMAT_R8G8_UNORM: return Nexus::Graphics::PixelFormat::R8_G8_UNorm;
			case VK_FORMAT_R8G8_SNORM: return Nexus::Graphics::PixelFormat::R8_G8_SNorm;
			case VK_FORMAT_R8G8_UINT: return Nexus::Graphics::PixelFormat::R8_G8_UInt;
			case VK_FORMAT_R8G8_SINT: return Nexus::Graphics::PixelFormat::R8_G8_SInt;

			case VK_FORMAT_R16G16_UNORM: return Nexus::Graphics::PixelFormat::R16_G16_UNorm;
			case VK_FORMAT_R16G16_SNORM: return Nexus::Graphics::PixelFormat::R16_G16_SNorm;
			case VK_FORMAT_R16G16_UINT: return Nexus::Graphics::PixelFormat::R16_G16_UInt;
			case VK_FORMAT_R16G16_SINT: return Nexus::Graphics::PixelFormat::R16_G16_SInt;
			case VK_FORMAT_R16G16_SFLOAT: return Nexus::Graphics::PixelFormat::R16_G16_Float;

			case VK_FORMAT_R32G32_UINT: return Nexus::Graphics::PixelFormat::R32_G32_UInt;
			case VK_FORMAT_R32G32_SINT: return Nexus::Graphics::PixelFormat::R32_G32_SInt;
			case VK_FORMAT_R32G32_SFLOAT: return Nexus::Graphics::PixelFormat::R32_G32_Float;

			case VK_FORMAT_R8G8B8A8_UNORM: return Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
			case VK_FORMAT_R8G8B8A8_SRGB: return Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB;
			case VK_FORMAT_B8G8R8A8_UNORM: return Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm;
			case VK_FORMAT_B8G8R8A8_SRGB: return Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB;
			case VK_FORMAT_R8G8B8A8_SNORM: return Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm;
			case VK_FORMAT_R8G8B8A8_UINT: return Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt;
			case VK_FORMAT_R8G8B8A8_SINT: return Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt;

			case VK_FORMAT_R16G16B16A16_UNORM: return Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm;
			case VK_FORMAT_R16G16B16A16_SNORM: return Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm;
			case VK_FORMAT_R16G16B16A16_UINT: return Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt;
			case VK_FORMAT_R16G16B16A16_SINT: return Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt;
			case VK_FORMAT_R16G16B16A16_SFLOAT: return Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float;

			case VK_FORMAT_R32G32B32A32_UINT: return Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt;
			case VK_FORMAT_R32G32B32A32_SINT: return Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt;
			case VK_FORMAT_R32G32B32A32_SFLOAT: return Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;

			case VK_FORMAT_D32_SFLOAT_S8_UINT: return Nexus::Graphics::PixelFormat::D32_SFloat_S8_UInt;
			case VK_FORMAT_D24_UNORM_S8_UINT: return Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;

			case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm;
			case VK_FORMAT_A2B10G10R10_UINT_PACK32: return Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt;
			case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return Nexus::Graphics::PixelFormat::R11_G11_B10_Float;

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	VkFormat GetShaderDataType(Nexus::Graphics::ShaderDataType type)
	{
		switch (type)
		{
			case Nexus::Graphics::ShaderDataType::R8_UInt: return VK_FORMAT_R8_UINT; break;
			case Nexus::Graphics::ShaderDataType::R8G8_UInt: return VK_FORMAT_R8G8_UINT; break;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_UInt: return VK_FORMAT_R8G8B8A8_UINT; break;

			case Nexus::Graphics::ShaderDataType::R8_UNorm: return VK_FORMAT_R8_UNORM; break;
			case Nexus::Graphics::ShaderDataType::R8G8_UNorm: return VK_FORMAT_R8G8_UNORM; break;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM; break;

			case Nexus::Graphics::ShaderDataType::R32_SFloat: return VK_FORMAT_R32_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::R32G32_SFloat: return VK_FORMAT_R32G32_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::R32G32B32_SFloat: return VK_FORMAT_R32G32B32_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat: return VK_FORMAT_R32G32B32A32_SFLOAT; break;

			case Nexus::Graphics::ShaderDataType::R16_SFloat: return VK_FORMAT_R16_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::R16G16_SFloat: return VK_FORMAT_R16G16_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_SFloat: return VK_FORMAT_R16G16B16A16_SFLOAT; break;

			case Nexus::Graphics::ShaderDataType::R32_SInt: return VK_FORMAT_R32_SINT; break;
			case Nexus::Graphics::ShaderDataType::R32G32_SInt: return VK_FORMAT_R32G32_SINT; break;
			case Nexus::Graphics::ShaderDataType::R32G32B32_SInt: return VK_FORMAT_R32G32B32_SINT; break;
			case Nexus::Graphics::ShaderDataType::R32G32B32A32_SInt: return VK_FORMAT_R32G32B32A32_SINT; break;

			case Nexus::Graphics::ShaderDataType::R8_SInt: return VK_FORMAT_R8_SINT; break;
			case Nexus::Graphics::ShaderDataType::R8G8_SInt: return VK_FORMAT_R8G8_SINT; break;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_SInt: return VK_FORMAT_R8G8B8A8_SINT; break;

			case Nexus::Graphics::ShaderDataType::R8_SNorm: return VK_FORMAT_R8_SNORM; break;
			case Nexus::Graphics::ShaderDataType::R8G8_SNorm: return VK_FORMAT_R8G8_SNORM; break;
			case Nexus::Graphics::ShaderDataType::R8G8B8A8_SNorm: return VK_FORMAT_R8G8B8A8_SNORM; break;

			case Nexus::Graphics::ShaderDataType::R16_SInt: return VK_FORMAT_R16_SINT; break;
			case Nexus::Graphics::ShaderDataType::R16G16_SInt: return VK_FORMAT_R16G16_SINT; break;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_SInt: return VK_FORMAT_R16G16B16A16_SINT; break;

			case Nexus::Graphics::ShaderDataType::R16_SNorm: return VK_FORMAT_R16_SNORM; break;
			case Nexus::Graphics::ShaderDataType::R16G16_SNorm: return VK_FORMAT_R16G16_SNORM; break;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_SNorm: return VK_FORMAT_R16G16B16A16_SNORM; break;

			case Nexus::Graphics::ShaderDataType::R32_UInt: return VK_FORMAT_R32_UINT; break;
			case Nexus::Graphics::ShaderDataType::R32G32_UInt: return VK_FORMAT_R32G32_UINT; break;
			case Nexus::Graphics::ShaderDataType::R32G32B32_UInt: return VK_FORMAT_R32G32B32_UINT; break;
			case Nexus::Graphics::ShaderDataType::R32G32B32A32_UInt: return VK_FORMAT_R32G32B32A32_UINT; break;

			case Nexus::Graphics::ShaderDataType::R16_UInt: return VK_FORMAT_R16_UINT; break;
			case Nexus::Graphics::ShaderDataType::R16G16_UInt: return VK_FORMAT_R16G16_UINT; break;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_UInt: return VK_FORMAT_R16G16B16A16_UINT; break;

			case Nexus::Graphics::ShaderDataType::R16_UNorm: return VK_FORMAT_R16_UNORM; break;
			case Nexus::Graphics::ShaderDataType::R16G16_UNorm: return VK_FORMAT_R16G16_UNORM; break;
			case Nexus::Graphics::ShaderDataType::R16G16B16A16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM; break;

			case Nexus::Graphics::ShaderDataType::A2B10G10R10_UNorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			case Nexus::Graphics::ShaderDataType::A2B10G10R10_UInt: return VK_FORMAT_A2B10G10R10_UINT_PACK32;

			default: throw std::runtime_error("Failed to find valid vertex buffer element type");
		}
	}

	void GetVkFilterFromNexusFormat(Nexus::Graphics::SamplerFilter filter, VkFilter &min, VkFilter &max, VkSamplerMipmapMode &mipmapMode)
	{
		switch (filter)
		{
			case Nexus::Graphics::SamplerFilter::Anisotropic:
				min		   = VK_FILTER_LINEAR;
				max		   = VK_FILTER_LINEAR;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint:
				min		   = VK_FILTER_NEAREST;
				max		   = VK_FILTER_NEAREST;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear:
				min		   = VK_FILTER_NEAREST;
				max		   = VK_FILTER_NEAREST;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint:
				min		   = VK_FILTER_NEAREST;
				max		   = VK_FILTER_LINEAR;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear:
				min		   = VK_FILTER_NEAREST;
				max		   = VK_FILTER_LINEAR;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint:
				min		   = VK_FILTER_LINEAR;
				max		   = VK_FILTER_NEAREST;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear:
				min		   = VK_FILTER_LINEAR;
				max		   = VK_FILTER_NEAREST;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint:
				min		   = VK_FILTER_LINEAR;
				max		   = VK_FILTER_LINEAR;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear:
				min		   = VK_FILTER_LINEAR;
				max		   = VK_FILTER_LINEAR;
				mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;

			default: throw std::runtime_error("Failed to find a valid sampler filter");
		}
	}

	VkSamplerAddressMode GetVkSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
	{
		switch (addressMode)
		{
			case Nexus::Graphics::SamplerAddressMode::Border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case Nexus::Graphics::SamplerAddressMode::Clamp: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case Nexus::Graphics::SamplerAddressMode::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case Nexus::Graphics::SamplerAddressMode::MirrorOnce: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			case Nexus::Graphics::SamplerAddressMode::Wrap: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			default: throw std::runtime_error("Failed to find valid sampler address mode");
		}
	}

	VkCompareOp GetCompareOp(Nexus::Graphics::ComparisonFunction function)
	{
		switch (function)
		{
			case Nexus::Graphics::ComparisonFunction::AlwaysPass: return VK_COMPARE_OP_ALWAYS;
			case Nexus::Graphics::ComparisonFunction::Equal: return VK_COMPARE_OP_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Greater: return VK_COMPARE_OP_GREATER;
			case Nexus::Graphics::ComparisonFunction::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Less: return VK_COMPARE_OP_LESS;
			case Nexus::Graphics::ComparisonFunction::LessEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Never: return VK_COMPARE_OP_NEVER;
			case Nexus::Graphics::ComparisonFunction::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
			default: throw std::runtime_error("Failed to find a valid comparison function");
		}
	}

	VkBlendOp GetVkBlendOp(Nexus::Graphics::BlendEquation function)
	{
		switch (function)
		{
			case Nexus::Graphics::BlendEquation::Add: return VK_BLEND_OP_ADD;
			case Nexus::Graphics::BlendEquation::Subtract: return VK_BLEND_OP_SUBTRACT;
			case Nexus::Graphics::BlendEquation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
			case Nexus::Graphics::BlendEquation::Min: return VK_BLEND_OP_MIN;
			case Nexus::Graphics::BlendEquation::Max: return VK_BLEND_OP_MAX;
			default: throw std::runtime_error("Failed to find a valid blend operation");
		}
	}

	VkStencilOp GetStencilOp(Nexus::Graphics::StencilOperation operation)
	{
		switch (operation)
		{
			case Nexus::Graphics::StencilOperation::Keep: return VK_STENCIL_OP_KEEP;
			case Nexus::Graphics::StencilOperation::Zero: return VK_STENCIL_OP_ZERO;
			case Nexus::Graphics::StencilOperation::Replace: return VK_STENCIL_OP_REPLACE;
			case Nexus::Graphics::StencilOperation::Increment: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case Nexus::Graphics::StencilOperation::Decrement: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case Nexus::Graphics::StencilOperation::Invert: return VK_STENCIL_OP_INVERT;
			default: throw std::runtime_error("Failed to find a valid stencil operation");
		}
	}

	VkBlendFactor GetVkBlendFactor(Nexus::Graphics::BlendFactor function)
	{
		switch (function)
		{
			case Nexus::Graphics::BlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
			case Nexus::Graphics::BlendFactor::One: return VK_BLEND_FACTOR_ONE;
			case Nexus::Graphics::BlendFactor::SourceColour: return VK_BLEND_FACTOR_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusSourceColour: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::DestinationColour: return VK_BLEND_FACTOR_DST_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationColour: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case Nexus::Graphics::BlendFactor::SourceAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::DestinationAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			case Nexus::Graphics::BlendFactor::FactorColour: return VK_BLEND_FACTOR_CONSTANT_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusFactorColour: VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			case Nexus::Graphics::BlendFactor::FactorAlpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusFactorAlpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

			default: throw std::runtime_error("Failed to find a valid blend factor");
		}
	}

	VkBorderColor GetVkBorderColor(Nexus::Graphics::BorderColor color)
	{
		switch (color)
		{
			case Nexus::Graphics::BorderColor::TransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			case Nexus::Graphics::BorderColor::OpaqueBlack: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			case Nexus::Graphics::BorderColor::OpaqueWhite: return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			default: throw std::runtime_error("Failed to find a valid border color");
		}
	}

	VkImageUsageFlagBits GetVkImageUsageFlags(Graphics::PixelFormat format, uint8_t usage)
	{
		VkImageUsageFlagBits flags = VkImageUsageFlagBits(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

		Graphics::PixelFormatType pixelFormatType = Graphics::GetPixelFormatType(format);
		if (pixelFormatType == Graphics::PixelFormatType::DepthStencil)
		{
			flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		if (usage & Nexus::Graphics::TextureUsage_RenderTarget)
		{
			flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		}

		// this is required to be set to use an VkImageView
		//  if (usage & Nexus::Graphics::TextureUsage_Sampled)
		{
			flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_SAMPLED_BIT);
		}

		if (usage & Nexus::Graphics::TextureUsage_Storage)
		{
			flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_STORAGE_BIT);
		}

		return flags;
	}

	VkImageCreateFlagBits GetVkImageCreateFlagBits(Graphics::TextureType textureType, uint8_t usage)
	{
		VkImageCreateFlagBits flags = VkImageCreateFlagBits();

		if (textureType == Graphics::TextureType::TextureCube)
		{
			flags = VkImageCreateFlagBits(flags | VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
		}

		return flags;
	}

	VkImageType GetVkImageType(Graphics::TextureType textureType)
	{
		switch (textureType)
		{
			case Graphics::TextureType::Texture1D: return VK_IMAGE_TYPE_1D;
			case Graphics::TextureType::Texture2D:
			case Graphics::TextureType::TextureCube: return VK_IMAGE_TYPE_2D;
			case Graphics::TextureType::Texture3D: return VK_IMAGE_TYPE_3D;
			default: throw std::runtime_error("Failed to find a valid image type");
		}
	}

	VkImageViewType GetVkImageViewType(const Graphics::TextureDescription &spec)
	{
		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
			{
				if (spec.DepthOrArrayLayers > 1)
				{
					return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				}
				else
				{
					return VK_IMAGE_VIEW_TYPE_1D;
				}
			}
			case Graphics::TextureType::Texture2D:
			{
				if (spec.DepthOrArrayLayers > 1)
				{
					return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				}
				else
				{
					return VK_IMAGE_VIEW_TYPE_2D;
				}
			}
			case Graphics::TextureType::Texture3D:
			{
				return VK_IMAGE_VIEW_TYPE_3D;
			}
			case Graphics::TextureType::TextureCube:
			{
				if (spec.DepthOrArrayLayers > 1)
				{
					return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
				}
				else
				{
					return VK_IMAGE_VIEW_TYPE_CUBE;
				}
			}
		}

		throw std::runtime_error("Failed to find a valid image view type");
	}

	bool IsSingleShaderStage(Graphics::ShaderStage stage)
	{
		uint16_t value = static_cast<uint16_t>(stage);
		return value != 0 && (value & (value - 1)) == 0;
	}

	VkShaderStageFlagBits GetVkShaderStageFlags(Nexus::Graphics::ShaderStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			case Nexus::Graphics::ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case Nexus::Graphics::ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
			case Nexus::Graphics::ShaderStage::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case Nexus::Graphics::ShaderStage::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case Nexus::Graphics::ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;

			case Nexus::Graphics::ShaderStage::RayGeneration: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			case Nexus::Graphics::ShaderStage::RayMiss: return VK_SHADER_STAGE_MISS_BIT_KHR;
			case Nexus::Graphics::ShaderStage::RayClosestHit: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			case Nexus::Graphics::ShaderStage::RayAnyHit: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
			case Nexus::Graphics::ShaderStage::RayIntersection: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

			case Nexus::Graphics::ShaderStage::Mesh: return VK_SHADER_STAGE_MESH_BIT_EXT;
			case Nexus::Graphics::ShaderStage::Task: return VK_SHADER_STAGE_TASK_BIT_EXT;

			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}

	VkShaderStageFlagBits GetVkShaderStageFlagsFromShaderStages(const Nexus::Graphics::ShaderStageFlags &flags)
	{
		VkShaderStageFlags vkStageFlags = 0;

		if (flags.HasFlag(Graphics::ShaderStage::Compute))
		{
			vkStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::Fragment))
		{
			vkStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::Geometry))
		{
			vkStageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::TessellationControl))
		{
			vkStageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::TessellationEvaluation))
		{
			vkStageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::Vertex))
		{
			vkStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::RayGeneration))
		{
			vkStageFlags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		}

		if (flags.HasFlag(Graphics::ShaderStage::RayMiss))
		{
			vkStageFlags |= VK_SHADER_STAGE_MISS_BIT_KHR;
		}

		if (flags.HasFlag(Graphics::ShaderStage::RayClosestHit))
		{
			vkStageFlags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		}

		if (flags.HasFlag(Graphics::ShaderStage::RayAnyHit))
		{
			vkStageFlags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		}

		if (flags.HasFlag(Graphics::ShaderStage::RayIntersection))
		{
			vkStageFlags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		}

		if (flags.HasFlag(Graphics::ShaderStage::Mesh))
		{
			vkStageFlags |= VK_SHADER_STAGE_MESH_BIT_EXT;
		}

		if (flags.HasFlag(Graphics::ShaderStage::Task))
		{
			vkStageFlags |= VK_SHADER_STAGE_TASK_BIT_EXT;
		}

		return VkShaderStageFlagBits(vkStageFlags);
	}

	VkIndexType GetVulkanIndexBufferFormat(Nexus::Graphics::IndexFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::IndexFormat::UInt8: return VK_INDEX_TYPE_UINT8_EXT;
			case Nexus::Graphics::IndexFormat::UInt16: return VK_INDEX_TYPE_UINT16;
			case Nexus::Graphics::IndexFormat::UInt32: return VK_INDEX_TYPE_UINT32;
			default: throw std::runtime_error("Failed to find a valid index buffer format");
		}
	}

	VkFrontFace GetFrontFace(Nexus::Graphics::FrontFace frontFace)
	{
		switch (frontFace)
		{
			case Nexus::Graphics::FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
			case Nexus::Graphics::FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			default: throw std::runtime_error("Failed to find a valid front face");
		}
	}

	VkBufferUsageFlags GetVkBufferUsage(const Graphics::DeviceBufferDescription &desc, Graphics::GraphicsDeviceVk *device)
	{
		VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if (desc.Usage & Graphics::BufferUsage::Vertex)
		{
			flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}

		if (desc.Usage & Graphics::BufferUsage::Index)
		{
			flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}

		if (desc.Usage & Graphics::BufferUsage::Uniform)
		{
			flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}

		if (desc.Usage & Graphics::BufferUsage::Storage)
		{
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if (desc.Usage & Graphics::BufferUsage::Indirect)
		{
			flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}

		if (device->IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
		{
			if (desc.Usage & Graphics::BufferUsage::AccelerationStructureStorage)
			{
				flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
			}

			if (desc.Usage & Graphics::BufferUsage::AccelerationStructureBuildInputReadOnly)
			{
				flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
			}
		}

		if (device->IsVersionGreaterThan(VK_VERSION_1_2) || device->IsExtensionSupported(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		}

		return flags;
	}

	VkBufferCreateInfo GetVkBufferCreateInfo(const Graphics::DeviceBufferDescription &desc, Graphics::GraphicsDeviceVk *device)
	{
		VkBufferUsageFlags bufferUsage = GetVkBufferUsage(desc, device);

		VkBufferCreateInfo createInfo = {};
		createInfo.sType			  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size				  = desc.SizeInBytes;
		createInfo.usage			  = bufferUsage;
		return createInfo;
	}

	VmaAllocationCreateInfo GetVmaAllocationCreateInfo(const Graphics::DeviceBufferDescription &desc, Graphics::GraphicsDeviceVk *device)
	{
		VmaAllocationCreateInfo createInfo = {};
		createInfo.usage				   = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		if (desc.Access == Graphics::BufferMemoryAccess::Upload || desc.Access == Graphics::BufferMemoryAccess::Readback)
		{
			createInfo.flags		 = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			createInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}

		return createInfo;
	}

	bool SetObjectName(VkDevice device, VkObjectType type, uint64_t objectHandle, const char *name)
	{
		VkDebugUtilsObjectNameInfoEXT nameInfo = {};
		nameInfo.sType						   = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.pNext						   = nullptr;
		nameInfo.objectType					   = type;
		nameInfo.objectHandle				   = objectHandle;
		nameInfo.pObjectName				   = name;

		auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
		if (func != nullptr)
		{
			if (func(device, &nameInfo) != VK_SUCCESS)
				return false;
		}

		return true;
	}

	uint32_t GetSampleCountFromVkSampleCountFlags(VkSampleCountFlags sampleCount)
	{
		uint32_t count = 0;
		while (sampleCount > 1)
		{
			sampleCount >>= 1;
			count++;
		}

		return 1 << count;
	}

	VkSampleCountFlagBits GetVkSampleCountFlagsFromSampleCount(uint32_t samples)
	{
		VkSampleCountFlagBits vkSampleCountFlag = static_cast<VkSampleCountFlagBits>(samples);
		return vkSampleCountFlag;
	}

	VkImageAspectFlagBits GetAspectFlags(Graphics::ImageAspect aspect)
	{
		switch (aspect)
		{
			case Graphics::ImageAspect::Colour: return VK_IMAGE_ASPECT_COLOR_BIT;
			case Graphics::ImageAspect::Depth: return VK_IMAGE_ASPECT_DEPTH_BIT;
			case Graphics::ImageAspect::Stencil: return VK_IMAGE_ASPECT_STENCIL_BIT;
			case Graphics::ImageAspect::DepthStencil: return VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
			default: throw std::runtime_error("Failed to find valid aspect flags");
		}
	}

	VkAccelerationStructureTypeKHR GetAccelerationStructureType(Graphics::AccelerationStructureType type)
	{
		switch (type)
		{
			case Graphics::AccelerationStructureType::BottomLevel: return VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			case Graphics::AccelerationStructureType::TopLevel: return VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
			default: throw std::runtime_error("Failed to find a valid acceleration structure type");
		}
	}

	VkBuildAccelerationStructureFlagsKHR GetAccelerationStructureFlags(uint8_t flags)
	{
		VkBuildAccelerationStructureFlagBitsKHR vulkanFlags = {};

		if (flags & Graphics::AccelerationStructureBuildFlags::AllowUpdate)
		{
			vulkanFlags = VkBuildAccelerationStructureFlagBitsKHR(vulkanFlags | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR);
		}

		if (flags & Graphics::AccelerationStructureBuildFlags::AllowCompaction)
		{
			vulkanFlags = VkBuildAccelerationStructureFlagBitsKHR(vulkanFlags | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
		}

		if (flags & Graphics::AccelerationStructureBuildFlags::PreferFastTrace)
		{
			vulkanFlags = VkBuildAccelerationStructureFlagBitsKHR(vulkanFlags | VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
		}

		if (flags & Graphics::AccelerationStructureBuildFlags::PreferFastBuild)
		{
			vulkanFlags = VkBuildAccelerationStructureFlagBitsKHR(vulkanFlags | VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR);
		}

		if (flags & Graphics::AccelerationStructureBuildFlags::MinimizeMemory)
		{
			vulkanFlags = VkBuildAccelerationStructureFlagBitsKHR(vulkanFlags | VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR);
		}

		return vulkanFlags;
	}

	VkBuildAccelerationStructureModeKHR GetAccelerationStructureBuildMode(Graphics::AccelerationStructureBuildMode mode)
	{
		switch (mode)
		{
			case Graphics::AccelerationStructureBuildMode::Build: return VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
			case Graphics::AccelerationStructureBuildMode::Update: return VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
			default: throw std::runtime_error("Failed to find a valid build mode");
		}
	}

	VkGeometryTypeKHR GetAccelerationStructureGeometryType(Graphics::GeometryType type)
	{
		switch (type)
		{
			case Graphics::GeometryType::AxisAlignedBoundingBoxes: return VK_GEOMETRY_TYPE_AABBS_KHR;
			case Graphics::GeometryType::Instance: return VK_GEOMETRY_TYPE_INSTANCES_KHR;
			case Graphics::GeometryType::Triangles: return VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			default: throw std::runtime_error("Failed to find a valid geometry type");
		}
	}

	VkGeometryFlagsKHR GetAccelerationStructureGeometryFlags(uint8_t flags)
	{
		VkGeometryFlagsKHR geometryFlags = {};

		if (flags & Graphics::AccelerationStructureGeometryFlags::Opaque)
		{
			geometryFlags = (VkGeometryFlagsKHR)(geometryFlags | VK_GEOMETRY_OPAQUE_BIT_KHR);
		}

		if (flags & Graphics::AccelerationStructureGeometryFlags::NoDuplicateAnyhit)
		{
			geometryFlags = (VkGeometryFlagsKHR)(geometryFlags | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
		}

		return geometryFlags;
	}

	VkAccelerationStructureGeometryDataKHR GetAccelerationStructureGeometryData(const Graphics::AccelerationStructureGeometryDescription &geometry)
	{
		switch (geometry.Type)
		{
			case Graphics::GeometryType::AxisAlignedBoundingBoxes:
			{
				Graphics::AccelerationStructureAABBGeometry aabbs = std::get<Graphics::AccelerationStructureAABBGeometry>(geometry.Geometry);

				VkAccelerationStructureGeometryAabbsDataKHR outAabbs = {};
				outAabbs.sType										 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
				outAabbs.pNext										 = nullptr;
				outAabbs.stride										 = aabbs.Stride;
				outAabbs.data										 = Vk::GetDeviceOrHostAddressConst(aabbs.AABBs);

				return VkAccelerationStructureGeometryDataKHR {.aabbs = outAabbs};
			}

			case Graphics::GeometryType::Instance:
			{
				Graphics::AccelerationStructureInstanceGeometry instances =
					std::get<Graphics::AccelerationStructureInstanceGeometry>(geometry.Geometry);

				VkAccelerationStructureGeometryInstancesDataKHR instanceGeometry = {};
				instanceGeometry.sType			 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
				instanceGeometry.pNext			 = nullptr;
				instanceGeometry.arrayOfPointers = instances.ArrayOfPointers;
				instanceGeometry.data			 = Vk::GetDeviceOrHostAddressConst(instances.InstanceBuffer);

				return VkAccelerationStructureGeometryDataKHR {.instances = instanceGeometry};
			}
			case Graphics::GeometryType::Triangles:
			{
				Graphics::AccelerationStructureTriangleGeometry triangles =
					std::get<Graphics::AccelerationStructureTriangleGeometry>(geometry.Geometry);

				VkAccelerationStructureGeometryTrianglesDataKHR triangleGeometry = {};
				triangleGeometry.sType		   = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
				triangleGeometry.pNext		   = nullptr;
				triangleGeometry.vertexFormat  = Vk::GetVulkanVertexFormat(triangles.VertexBufferFormat);
				triangleGeometry.vertexData	   = Vk::GetDeviceOrHostAddressConst(triangles.VertexBuffer);
				triangleGeometry.vertexStride  = triangles.VertexBufferStride;
				triangleGeometry.maxVertex	   = triangles.VertexCount - 1;
				triangleGeometry.indexType	   = Vk::GetVulkanIndexBufferFormat(triangles.IndexBufferFormat);
				triangleGeometry.indexData	   = Vk::GetDeviceOrHostAddressConst(triangles.IndexBuffer);
				triangleGeometry.transformData = Vk::GetDeviceOrHostAddressConst(triangles.TransformBuffer);

				return VkAccelerationStructureGeometryDataKHR {.triangles = triangleGeometry};
			}
			default: throw std::runtime_error("Failed to get geometry");
		}
	}

	VkDeviceOrHostAddressKHR GetDeviceOrHostAddress(Graphics::DeviceBufferAddress address)
	{
		if (address.Buffer)
		{
			Ref<Graphics::DeviceBufferVk> vulkanBuffer	= std::dynamic_pointer_cast<Graphics::DeviceBufferVk>(address.Buffer);
			VkDeviceAddress				  deviceAddress = vulkanBuffer->GetDeviceAddress() + address.Offset;
			return VkDeviceOrHostAddressKHR {.deviceAddress = deviceAddress};
		}
		else
		{
			return VkDeviceOrHostAddressKHR {.deviceAddress = 0};
		}
	}

	VkDeviceOrHostAddressConstKHR GetDeviceOrHostAddressConst(Graphics::DeviceBufferAddress address)
	{
		if (address.Buffer)
		{
			Ref<Graphics::DeviceBufferVk> vulkanBuffer	= std::dynamic_pointer_cast<Graphics::DeviceBufferVk>(address.Buffer);
			VkDeviceAddress				  deviceAddress = vulkanBuffer->GetDeviceAddress() + address.Offset;
			return VkDeviceOrHostAddressConstKHR {.deviceAddress = deviceAddress};
		}
		else
		{
			return VkDeviceOrHostAddressConstKHR {.deviceAddress = 0};
		}
	}

	VkFormat GetVulkanVertexFormat(Graphics::VertexFormat format)
	{
		switch (format)
		{
			case Graphics::VertexFormat::R32G32_SFloat: return VK_FORMAT_R32G32_SFLOAT;
			case Graphics::VertexFormat::R32G32B32_SFloat: return VK_FORMAT_R32G32B32_SFLOAT;
			case Graphics::VertexFormat::R16G16_SFloat: return VK_FORMAT_R16G16_SFLOAT;
			case Graphics::VertexFormat::R16G16B16A16_SFloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
			case Graphics::VertexFormat::R16G16_SNorm: return VK_FORMAT_R16G16_SNORM;
			case Graphics::VertexFormat::R16G16B16A16_SNorm: return VK_FORMAT_R16G16B16A16_SNORM;
			case Graphics::VertexFormat::R16G16B16A16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM;
			case Graphics::VertexFormat::R16G16_UNorm: return VK_FORMAT_R16G16_UNORM;
			case Graphics::VertexFormat::R10G10B10A2_UNorm: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
			case Graphics::VertexFormat::R8G8_UNorm: return VK_FORMAT_R8G8_UNORM;
			case Graphics::VertexFormat::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
			case Graphics::VertexFormat::R8G8_SNorm: return VK_FORMAT_R8G8_SNORM;
			default: throw std::runtime_error("Failed to find a valid vertex format");
		}
	}

	std::vector<VkAccelerationStructureGeometryKHR> GetVulkanAccelerationStructureGeometries(
		const Graphics::AccelerationStructureGeometryBuildDescription &description)
	{
		std::vector<VkAccelerationStructureGeometryKHR> geometries;
		geometries.reserve(description.Geometry.size());

		for (const auto &geometry : description.Geometry)
		{
			VkAccelerationStructureGeometryKHR asGeometry = {};
			asGeometry.sType							  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			asGeometry.pNext							  = nullptr;
			asGeometry.flags							  = Vk::GetAccelerationStructureGeometryFlags(geometry.Flags);
			asGeometry.geometryType						  = Vk::GetAccelerationStructureGeometryType(geometry.Type);
			asGeometry.geometry							  = Vk::GetAccelerationStructureGeometryData(geometry);
			geometries.push_back(asGeometry);
		}

		return geometries;
	}

	VkAccelerationStructureBuildGeometryInfoKHR GetGeometryBuildInfo(const Graphics::AccelerationStructureGeometryBuildDescription &description,
																	 const std::vector<VkAccelerationStructureGeometryKHR>		   &geometry)
	{
		VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {};
		buildInfo.sType										  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildInfo.pNext										  = nullptr;
		buildInfo.type										  = Vk::GetAccelerationStructureType(description.Type);
		buildInfo.flags										  = Vk::GetAccelerationStructureFlags(description.Flags);
		buildInfo.mode										  = Vk::GetAccelerationStructureBuildMode(description.Mode);
		buildInfo.geometryCount								  = geometry.size();
		buildInfo.pGeometries								  = geometry.data();
		buildInfo.srcAccelerationStructure					  = VK_NULL_HANDLE;
		buildInfo.dstAccelerationStructure					  = VK_NULL_HANDLE;
		buildInfo.scratchData.deviceAddress					  = 0;

		if (description.Source)
		{
			Ref<Graphics::AccelerationStructureVk> accelerationStructure =
				std::dynamic_pointer_cast<Graphics::AccelerationStructureVk>(description.Source);
			buildInfo.srcAccelerationStructure = accelerationStructure->GetHandle();
		}

		if (description.Destination)
		{
			Ref<Graphics::AccelerationStructureVk> accelerationStructure =
				std::dynamic_pointer_cast<Graphics::AccelerationStructureVk>(description.Destination);
			buildInfo.dstAccelerationStructure = accelerationStructure->GetHandle();
		}

		buildInfo.scratchData = Vk::GetDeviceOrHostAddress(description.ScratchBuffer);

		return buildInfo;
	}

	VkAccelerationStructureBuildRangeInfoKHR GetAccelerationStructureBuildRange(Graphics::AccelerationStructureBuildRange range)
	{
		VkAccelerationStructureBuildRangeInfoKHR rangeInfo = {};
		rangeInfo.primitiveCount						   = range.PrimitiveCount;
		rangeInfo.primitiveOffset						   = range.PrimitiveOffset;
		rangeInfo.firstVertex							   = range.FirstVertex;
		rangeInfo.transformOffset						   = range.TransformOffset;
		return rangeInfo;
	}

	VkRenderPass CreateVkRenderPass(VkDevice device, const VulkanRenderPassDescription &desc)
	{
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference>	 colourAttachmentReferences;
		VkAttachmentReference				 depthAttachmentReference;
		VkAttachmentReference				 resolveAttachmentReference;
		size_t								 attachmentIndex = 0;

		for (VkFormat colourAttachmentFormat : desc.ColourAttachments)
		{
			VkAttachmentDescription colourAttachment = {};
			colourAttachment.format					 = colourAttachmentFormat;
			colourAttachment.samples				 = desc.Samples;
			colourAttachment.loadOp					 = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.storeOp				 = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.stencilLoadOp			 = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.stencilStoreOp			 = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.initialLayout			 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			if (desc.IsSwapchain && desc.Samples != 1)
			{
				colourAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else
			{
				colourAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			attachments.push_back(colourAttachment);

			VkAttachmentReference attachmentReference = {};
			attachmentReference.attachment			  = attachmentIndex;
			attachmentReference.layout				  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colourAttachmentReferences.push_back(attachmentReference);

			attachmentIndex++;
		}

		if (desc.DepthFormat)
		{
			VkFormat depthFormatVal = desc.DepthFormat.value();

			VkAttachmentDescription depthAttachment = {};
			depthAttachment.format					= depthFormatVal;
			depthAttachment.samples					= desc.Samples;
			depthAttachment.loadOp					= VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.storeOp					= VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.stencilLoadOp			= VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.stencilStoreOp			= VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.initialLayout			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			depthAttachment.finalLayout				= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments.push_back(depthAttachment);

			depthAttachmentReference.attachment = attachmentIndex;
			depthAttachmentReference.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachmentIndex++;
		}

		if (desc.ResolveFormat)
		{
			VkFormat resolveFormatVal = desc.ResolveFormat.value();

			VkAttachmentDescription resolveAttachment = {};
			resolveAttachment.format				  = resolveFormatVal;
			resolveAttachment.samples				  = VK_SAMPLE_COUNT_1_BIT;
			resolveAttachment.loadOp				  = VK_ATTACHMENT_LOAD_OP_LOAD;
			resolveAttachment.storeOp				  = VK_ATTACHMENT_STORE_OP_STORE;
			resolveAttachment.stencilLoadOp			  = VK_ATTACHMENT_LOAD_OP_LOAD;
			resolveAttachment.stencilStoreOp		  = VK_ATTACHMENT_STORE_OP_STORE;
			resolveAttachment.initialLayout			  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			resolveAttachment.finalLayout			  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachments.push_back(resolveAttachment);

			resolveAttachmentReference.attachment = attachmentIndex;
			resolveAttachmentReference.layout	  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachmentIndex++;
		}

		VkSubpassDescription subpassDescription	   = {};
		subpassDescription.flags				   = 0;
		subpassDescription.pipelineBindPoint	   = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount	   = colourAttachmentReferences.size();
		subpassDescription.pColorAttachments	   = colourAttachmentReferences.data();
		subpassDescription.pDepthStencilAttachment = nullptr;

		if (desc.DepthFormat)
		{
			subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
		}

		subpassDescription.inputAttachmentCount	   = 0;
		subpassDescription.pInputAttachments	   = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments	   = nullptr;
		subpassDescription.pResolveAttachments	   = nullptr;

		if (desc.ResolveFormat)
		{
			subpassDescription.pResolveAttachments = &resolveAttachmentReference;
		}

		std::vector<VkSubpassDependency> dependencies(1);
		dependencies[0].srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass		= 0;
		dependencies[0].srcStageMask	= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask	= VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType				  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount		  = (uint32_t)attachments.size();
		createInfo.pAttachments			  = attachments.data();
		createInfo.subpassCount			  = 1;
		createInfo.pSubpasses			  = &subpassDescription;
		createInfo.dependencyCount		  = (uint32_t)dependencies.size();
		createInfo.pDependencies		  = dependencies.data();

		VkRenderPass renderPass = VK_NULL_HANDLE;
		NX_VALIDATE(vkCreateRenderPass(device, &createInfo, nullptr, &renderPass) == VK_SUCCESS, "Failed to create render pass");
		return renderPass;
	}

	VkRenderPass CreateVkRenderPass2(PFN_vkCreateRenderPass2KHR func, VkDevice device, const VulkanRenderPassDescription &desc)
	{
		std::vector<VkAttachmentDescription2KHR> attachments				= {};
		std::vector<VkAttachmentReference2KHR>	 colourAttachmentReferences = {};
		VkAttachmentReference2KHR				 depthAttachmentReference;
		VkAttachmentReference2KHR				 resolveAttachmentReference;
		size_t									 attachmentIndex = 0;

		for (VkFormat colourAttachmentFormat : desc.ColourAttachments)
		{
			VkAttachmentDescription2KHR colourAttachment = {};
			colourAttachment.sType						 = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
			colourAttachment.pNext						 = nullptr;
			colourAttachment.flags						 = 0;
			colourAttachment.format						 = colourAttachmentFormat;
			colourAttachment.samples					 = desc.Samples;
			colourAttachment.loadOp						 = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.storeOp					 = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.stencilLoadOp				 = VK_ATTACHMENT_LOAD_OP_LOAD;
			colourAttachment.stencilStoreOp				 = VK_ATTACHMENT_STORE_OP_STORE;
			colourAttachment.initialLayout				 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colourAttachment.finalLayout				 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachments.push_back(colourAttachment);

			VkAttachmentReference2KHR attachmentReference = {};
			attachmentReference.sType					  = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
			attachmentReference.pNext					  = nullptr;
			attachmentReference.attachment				  = attachmentIndex;
			attachmentReference.layout					  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colourAttachmentReferences.push_back(attachmentReference);
			attachmentIndex++;
		}

		if (desc.DepthFormat)
		{
			VkFormat depthFormat = desc.DepthFormat.value();

			VkAttachmentDescription2KHR depthAttachment = {};
			depthAttachment.sType						= VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
			depthAttachment.pNext						= nullptr;
			depthAttachment.flags						= 0;
			depthAttachment.format						= depthFormat;
			depthAttachment.samples						= desc.Samples;
			depthAttachment.loadOp						= VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.storeOp						= VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.stencilLoadOp				= VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.stencilStoreOp				= VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.initialLayout				= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			depthAttachment.finalLayout					= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments.push_back(depthAttachment);

			depthAttachmentReference.sType		= VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
			depthAttachmentReference.pNext		= nullptr;
			depthAttachmentReference.attachment = attachmentIndex;
			depthAttachmentReference.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachmentIndex++;
		}

		if (desc.ResolveFormat)
		{
			VkFormat resolveFormat = desc.ResolveFormat.value();

			VkAttachmentDescription2KHR resolveAttachment = {};
			resolveAttachment.sType						  = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
			resolveAttachment.pNext						  = nullptr;
			resolveAttachment.flags						  = 0;
			resolveAttachment.format					  = resolveFormat;
			resolveAttachment.samples					  = VK_SAMPLE_COUNT_1_BIT;
			resolveAttachment.loadOp					  = VK_ATTACHMENT_LOAD_OP_LOAD;
			resolveAttachment.storeOp					  = VK_ATTACHMENT_STORE_OP_STORE;
			resolveAttachment.stencilLoadOp				  = VK_ATTACHMENT_LOAD_OP_LOAD;
			resolveAttachment.stencilStoreOp			  = VK_ATTACHMENT_STORE_OP_STORE;
			resolveAttachment.initialLayout				  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			resolveAttachment.finalLayout				  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachments.push_back(resolveAttachment);

			resolveAttachmentReference.sType	  = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
			resolveAttachmentReference.pNext	  = nullptr;
			resolveAttachmentReference.attachment = attachmentIndex;
			resolveAttachmentReference.layout	  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachmentIndex++;
		}

		VkSubpassDescription2KHR subpassDescription = {};
		subpassDescription.sType					= VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
		subpassDescription.pNext					= nullptr;
		subpassDescription.flags					= 0;
		subpassDescription.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount		= colourAttachmentReferences.size();
		subpassDescription.pColorAttachments		= colourAttachmentReferences.data();
		subpassDescription.pDepthStencilAttachment	= nullptr;

		if (desc.DepthFormat)
		{
			subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
		}

		subpassDescription.inputAttachmentCount	   = 0;
		subpassDescription.pInputAttachments	   = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments	   = nullptr;
		subpassDescription.pResolveAttachments	   = nullptr;

		if (desc.ResolveFormat)
		{
			subpassDescription.pResolveAttachments = &resolveAttachmentReference;
		}

		std::vector<VkSubpassDependency2KHR> dependencies(1);
		dependencies[0].sType			= VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR;
		dependencies[0].pNext			= nullptr;
		dependencies[0].srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass		= 0;
		dependencies[0].srcStageMask	= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask	= VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo2KHR createInfo = {};
		createInfo.sType					  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
		createInfo.pNext					  = nullptr;
		createInfo.flags					  = 0;
		createInfo.attachmentCount			  = attachments.size();
		createInfo.pAttachments				  = attachments.data();
		createInfo.subpassCount				  = 1;
		createInfo.pSubpasses				  = &subpassDescription;
		createInfo.dependencyCount			  = dependencies.size();
		createInfo.pDependencies			  = dependencies.data();
		createInfo.correlatedViewMaskCount	  = 0;
		createInfo.pCorrelatedViewMasks		  = nullptr;

		VkRenderPass renderPass = VK_NULL_HANDLE;
		NX_VALIDATE(func(device, &createInfo, nullptr, &renderPass) == VK_SUCCESS, "Failed to create render pass");
		return renderPass;
	}

	VkRenderPass CreateRenderPass(Graphics::GraphicsDeviceVk *device, const VulkanRenderPassDescription &desc)
	{
		const Graphics::DeviceExtensionFunctions &functions = device->GetExtensionFunctions();
		if (functions.vkCreateRenderPass2KHR)
		{
			return CreateVkRenderPass2(functions.vkCreateRenderPass2KHR, device->GetVkDevice(), desc);
		}
		return CreateVkRenderPass(device->GetVkDevice(), desc);
	}

	VkFramebuffer CreateFramebuffer(VkDevice device, const VulkanFramebufferDescription &desc)
	{
		std::vector<VkImageView> attachments = {};

		for (VkImageView colourView : desc.ColourImageViews) { attachments.push_back(colourView); }

		if (desc.DepthImageView)
		{
			attachments.push_back(desc.DepthImageView);
		}

		if (desc.ResolveImageView)
		{
			attachments.push_back(desc.ResolveImageView);
		}

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType				   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass			   = desc.VulkanRenderPass;
		createInfo.attachmentCount		   = (uint32_t)attachments.size();
		createInfo.pAttachments			   = attachments.data();
		createInfo.width				   = desc.Width;
		createInfo.height				   = desc.Height;
		createInfo.layers				   = 1;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;

		NX_VALIDATE(vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer) == VK_SUCCESS, "Failed to create framebuffer");

		return framebuffer;
	}

	VkPipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module)
	{
		VkPipelineShaderStageCreateInfo createInfo = {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.stage						   = stage;
		createInfo.module						   = module;
		createInfo.pName						   = "main";
		return createInfo;
	}

	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology)
	{
		VkPipelineInputAssemblyStateCreateInfo info = {};
		info.sType									= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.pNext									= nullptr;
		info.topology								= topology;
		info.primitiveRestartEnable					= VK_FALSE;
		return info;
	}

	VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(const Graphics::RasterizerStateDescription &rasterizerDesc)
	{
		VkPolygonMode	polygonMode	 = Vk::GetPolygonMode(rasterizerDesc.TriangleFillMode);
		VkCullModeFlags cullingFlags = Vk::GetCullMode(rasterizerDesc.TriangleCullMode);
		VkFrontFace		frontFace	 = Vk::GetFrontFace(rasterizerDesc.TriangleFrontFace);

		VkPipelineRasterizationStateCreateInfo info = {};
		info.sType									= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.pNext									= nullptr;
		info.depthClampEnable						= VK_FALSE;
		info.rasterizerDiscardEnable				= VK_FALSE;
		info.polygonMode							= polygonMode;
		info.lineWidth								= 1.0f;
		info.cullMode								= cullingFlags;
		info.depthBiasEnable						= VK_FALSE;
		info.depthBiasConstantFactor				= 0.0f;
		info.depthBiasClamp							= 0.0f;
		info.depthBiasSlopeFactor					= 0.0f;
		info.frontFace								= frontFace;

		return info;
	}

	VkPipelineMultisampleStateCreateInfo CreateMultisampleStateCreateInfo(uint32_t sampleCount)
	{
		VkSampleCountFlagBits samples = Vk::GetVkSampleCountFlagsFromSampleCount(sampleCount);

		VkPipelineMultisampleStateCreateInfo info = {};
		info.sType								  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.pNext								  = nullptr;
		info.sampleShadingEnable				  = VK_FALSE;
		info.minSampleShading					  = 0.0f;
		info.rasterizationSamples				  = samples;
		info.minSampleShading					  = 1.0f;
		info.pSampleMask						  = nullptr;
		info.alphaToCoverageEnable				  = VK_FALSE;
		info.alphaToOneEnable					  = VK_FALSE;
		return info;
	}

	std::vector<VkPipelineColorBlendAttachmentState> CreateColorBlendAttachmentStates(
		uint32_t											  colourAttachmentCount,
		const std::array<Graphics::BlendStateDescription, 8> &blendStates)
	{
		std::vector<VkPipelineColorBlendAttachmentState> colourBlendStates;

		for (size_t i = 0; i < colourAttachmentCount; i++)
		{
			VkColorComponentFlags writeMask = {};
			if (blendStates[i].PixelWriteMask.Red)
			{
				writeMask |= VK_COLOR_COMPONENT_R_BIT;
			}
			if (blendStates[i].PixelWriteMask.Green)
			{
				writeMask |= VK_COLOR_COMPONENT_G_BIT;
			}
			if (blendStates[i].PixelWriteMask.Blue)
			{
				writeMask |= VK_COLOR_COMPONENT_B_BIT;
			}
			if (blendStates[i].PixelWriteMask.Alpha)
			{
				writeMask |= VK_COLOR_COMPONENT_A_BIT;
			}

			VkPipelineColorBlendAttachmentState blendState = {};
			blendState.colorWriteMask					   = writeMask;
			blendState.blendEnable						   = blendStates[i].EnableBlending;
			blendState.srcColorBlendFactor				   = Vk::GetVkBlendFactor(blendStates[i].SourceColourBlend);
			blendState.dstColorBlendFactor				   = Vk::GetVkBlendFactor(blendStates[i].DestinationColourBlend);
			blendState.colorBlendOp						   = Vk::GetVkBlendOp(blendStates[i].ColorBlendFunction);
			blendState.srcAlphaBlendFactor				   = Vk::GetVkBlendFactor(blendStates[i].SourceAlphaBlend);
			blendState.dstAlphaBlendFactor				   = Vk::GetVkBlendFactor(blendStates[i].DestinationAlphaBlend);
			blendState.alphaBlendOp						   = Vk::GetVkBlendOp(blendStates[i].AlphaBlendFunction);
			colourBlendStates.push_back(blendState);
		}

		return colourBlendStates;
	}

	VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateCreateInfo(const Graphics::DepthStencilDescription &depthStencilDesc)
	{
		VkPipelineDepthStencilStateCreateInfo info = {};
		info.sType								   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.pNext								   = nullptr;
		info.depthTestEnable					   = depthStencilDesc.EnableDepthTest;
		info.depthWriteEnable					   = depthStencilDesc.EnableDepthWrite;
		info.depthCompareOp						   = Vk::GetCompareOp(depthStencilDesc.DepthComparisonFunction);

		if (depthStencilDesc.EnableDepthsBoundsTest)
		{
			info.depthBoundsTestEnable = VK_TRUE;
		}
		else
		{
			info.depthBoundsTestEnable = VK_FALSE;
		}

		info.minDepthBounds = depthStencilDesc.MinDepth;
		info.maxDepthBounds = depthStencilDesc.MaxDepth;

		info.stencilTestEnable = depthStencilDesc.EnableStencilTest;

		info.back.writeMask	  = depthStencilDesc.StencilWriteMask;
		info.back.compareMask = depthStencilDesc.StencilCompareMask;
		info.back.reference	  = depthStencilDesc.StencilReference;
		info.back.compareOp	  = Vk::GetCompareOp(depthStencilDesc.Back.StencilComparisonFunction);
		info.back.failOp	  = Vk::GetStencilOp(depthStencilDesc.Back.StencilFailOperation);
		info.back.passOp	  = Vk::GetStencilOp(depthStencilDesc.Back.StencilSuccessDepthSuccessOperation);
		info.back.depthFailOp = Vk::GetStencilOp(depthStencilDesc.Back.StencilSuccessDepthFailOperation);

		info.front.writeMask   = depthStencilDesc.StencilWriteMask;
		info.front.compareMask = depthStencilDesc.StencilCompareMask;
		info.front.reference   = depthStencilDesc.StencilReference;
		info.front.compareOp   = Vk::GetCompareOp(depthStencilDesc.Front.StencilComparisonFunction);
		info.front.failOp	   = Vk::GetStencilOp(depthStencilDesc.Front.StencilFailOperation);
		info.front.passOp	   = Vk::GetStencilOp(depthStencilDesc.Front.StencilSuccessDepthSuccessOperation);
		info.front.depthFailOp = Vk::GetStencilOp(depthStencilDesc.Front.StencilSuccessDepthFailOperation);

		return info;
	}

	VkPrimitiveTopology GetPrimitiveTopology(Graphics::Topology topology)
	{
		switch (topology)
		{
			case Nexus::Graphics::Topology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
			case Nexus::Graphics::Topology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
			case Nexus::Graphics::Topology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
			case Nexus::Graphics::Topology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
			case Nexus::Graphics::Topology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
			default: throw std::runtime_error("An invalid primitive topology was entered"); break;
		}

		return VkPrimitiveTopology();
	}

	VkPolygonMode GetPolygonMode(Graphics::FillMode fillMode)
	{
		switch (fillMode)
		{
			case Nexus::Graphics::FillMode::Solid: return VK_POLYGON_MODE_FILL; break;
			case Nexus::Graphics::FillMode::Wireframe: return VK_POLYGON_MODE_LINE; break;
			default: throw std::runtime_error("An invalid fill mode was entered"); break;
		}
	}

	VkCullModeFlags GetCullMode(Graphics::CullMode cullMode)
	{
		switch (cullMode)
		{
			case Nexus::Graphics::CullMode::CullNone: return VK_CULL_MODE_NONE; break;
			case Nexus::Graphics::CullMode::Back: return VK_CULL_MODE_BACK_BIT; break;
			case Nexus::Graphics::CullMode::Front: return VK_CULL_MODE_FRONT_BIT; break;
			default: throw std::runtime_error("An invalid culling mode was entered"); break;
		}
	}

	void CreateVertexInputLayout(const std::vector<Graphics::VertexBufferLayout> &layouts,
								 std::vector<VkVertexInputAttributeDescription>	 &attributeDescriptions,
								 std::vector<VkVertexInputBindingDescription>	 &inputBindingDescriptions)
	{
		uint32_t elementIndex = 0;
		for (uint32_t layoutIndex = 0; layoutIndex < layouts.size(); layoutIndex++)
		{
			const auto &layout = layouts.at(layoutIndex);

			// create attribute descriptions
			for (uint32_t i = 0; i < layout.GetNumberOfElements(); i++)
			{
				const auto &element = layout.GetElement(i);

				VkVertexInputAttributeDescription attributeDescription = {};
				attributeDescription.binding						   = layoutIndex;
				attributeDescription.location						   = elementIndex;
				attributeDescription.format							   = Vk::GetShaderDataType(element.Type);
				attributeDescription.offset							   = element.Offset;
				attributeDescriptions.push_back(attributeDescription);

				elementIndex++;
			}

			// create binding descriptions
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding						   = layoutIndex;
			bindingDescription.stride						   = layout.GetStride();
			bindingDescription.inputRate = layout.IsInstanceBuffer() ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
			inputBindingDescriptions.push_back(bindingDescription);
		}
	}

	VkPipelineShaderStageCreateInfo CreateShaderStageCreateInfo(Nexus::Ref<Nexus::Graphics::ShaderModuleVk> module)
	{
		VkPipelineShaderStageCreateInfo createInfo = {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.stage						   = Vk::GetVkShaderStageFlags(module->GetModuleSpecification().ShadingStage);
		createInfo.module						   = module->GetShaderModule();
		createInfo.pName						   = "main";
		return createInfo;
	}

	VkDescriptorType GetDescriptorType(Graphics::ShaderResource resource)
	{
		switch (resource.Type)
		{
			case Graphics::ResourceType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case Graphics::ResourceType::Texture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case Graphics::ResourceType::UniformTextureBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			case Graphics::ResourceType::StorageTextureBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			case Graphics::ResourceType::Sampler:
			case Graphics::ResourceType::ComparisonSampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
			case Graphics::ResourceType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case Graphics::ResourceType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case Graphics::ResourceType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case Graphics::ResourceType::AccelerationStructure: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			default: throw std::runtime_error("Could not find a valid descriptor type");
		}
	}

	VkPipelineLayout CreatePipelineLayout(Graphics::Pipeline				   *pipeline,
										  Graphics::GraphicsDeviceVk		   *device,
										  std::vector<VkDescriptorSetLayout>   &descriptorSetLayouts,
										  std::map<VkDescriptorType, uint32_t> &descriptorCounts)
	{
		// retrieve the resources that are referenced by the shaders
		const auto &shaderResources = pipeline->GetRequiredShaderResources();

		// create storage for descriptor set layout bindings
		std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> sets;

		// iterate through all resources and convert them into Vulkan structures
		for (const auto &[name, resourceInfo] : shaderResources)
		{
			VkDescriptorType descriptorType = GetDescriptorType(resourceInfo);

			VkDescriptorSetLayoutBinding &layoutBinding = sets[resourceInfo.Set].emplace_back();
			layoutBinding.binding						= resourceInfo.Binding;
			layoutBinding.descriptorCount				= resourceInfo.ResourceCount;
			layoutBinding.descriptorType				= descriptorType;
			layoutBinding.stageFlags					= Vk::GetVkShaderStageFlagsFromShaderStages(resourceInfo.Stage);
			layoutBinding.pImmutableSamplers			= nullptr;

			descriptorCounts[descriptorType] += resourceInfo.ResourceCount;
		}

		// iterate through each vector of descriptor set layout bindings and create the layout
		for (const auto &[setIndex, layoutBindings] : sets)
		{
			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType						   = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.pNext						   = nullptr;
			createInfo.flags						   = 0;
			createInfo.pBindings					   = layoutBindings.data();
			createInfo.bindingCount					   = layoutBindings.size();

			VkDescriptorSetLayout &layout = descriptorSetLayouts.emplace_back();
			NX_VALIDATE(vkCreateDescriptorSetLayout(device->GetVkDevice(), &createInfo, nullptr, &layout) == VK_SUCCESS,
						"Failed to create descriptor set layout");
		}

		// create the actual VkPipelineLayout
		VkPipelineLayout layout;

		VkPipelineLayoutCreateInfo info = {};
		info.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext						= nullptr;
		info.flags						= 0;
		info.setLayoutCount				= descriptorSetLayouts.size();
		info.pSetLayouts				= descriptorSetLayouts.data();
		info.pushConstantRangeCount		= 0;
		info.pPushConstantRanges		= nullptr;

		NX_VALIDATE(vkCreatePipelineLayout(device->GetVkDevice(), &info, nullptr, &layout) == VK_SUCCESS, "Failed to create pipeline layout");

		return layout;
	}

	VkPipeline CreateGraphicsPipeline(VkRenderPass											  renderPass,
									  Graphics::GraphicsDeviceVk							 *device,
									  const Graphics::DepthStencilDescription				 &depthStencilDesc,
									  const Graphics::RasterizerStateDescription			 &rasterizerDesc,
									  uint32_t												  samples,
									  const std::vector<VkPipelineShaderStageCreateInfo>	 &shaderStages,
									  uint32_t												  colourTargetCount,
									  const std::array<Graphics::PixelFormat, 8>			 &colourFormats,
									  const std::array<Graphics::BlendStateDescription, 8>	 &blendStates,
									  Graphics::PixelFormat									  depthFormat,
									  VkPipelineLayout										  pipelineLayout,
									  Graphics::Topology									  topology,
									  const std::vector<Nexus::Graphics::VertexBufferLayout> &layouts)
	{
		const Graphics::VulkanDeviceFeatures &deviceFeatures = device->GetDeviceFeatures();

		VkPipelineDepthStencilStateCreateInfo  depthStencilInfo = CreatePipelineDepthStencilStateCreateInfo(depthStencilDesc);
		VkPipelineRasterizationStateCreateInfo rasterizerInfo	= Vk::CreateRasterizationStateCreateInfo(rasterizerDesc);

		VkPipelineMultisampleStateCreateInfo multisampleInfo = CreateMultisampleStateCreateInfo(samples);

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType								= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext								= nullptr;

		viewportState.viewportCount = 1;
		viewportState.pViewports	= nullptr;
		viewportState.scissorCount	= 1;
		viewportState.pScissors		= nullptr;

		std::vector<VkPipelineColorBlendAttachmentState> vkBlendStates = Vk::CreateColorBlendAttachmentStates(colourTargetCount, blendStates);

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType								  = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext								  = nullptr;
		colorBlending.logicOpEnable						  = VK_FALSE;
		colorBlending.logicOp							  = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount					  = vkBlendStates.size();
		colorBlending.pAttachments						  = vkBlendStates.data();

		// create vertex input layout
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		std::vector<VkVertexInputBindingDescription>   bindingDescriptions;
		Vk::CreateVertexInputLayout(layouts, attributeDescriptions, bindingDescriptions);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType								 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.pNext								 = nullptr;
		vertexInputInfo.vertexBindingDescriptionCount		 = bindingDescriptions.size();
		vertexInputInfo.pVertexBindingDescriptions			 = bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount		 = attributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions		 = attributeDescriptions.data();

		VkPrimitiveTopology					   primitiveTopology = Vk::GetPrimitiveTopology(topology);
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = CreateInputAssemblyCreateInfo(primitiveTopology);

		std::vector<VkFormat> vkColourFormats;

		for (uint32_t i = 0; i < colourTargetCount; i++) { vkColourFormats.push_back(Vk::GetVkPixelDataFormat(colourFormats[i])); }

		VkFormat depthStencilFormat = Vk::GetVkPixelDataFormat(depthFormat);

		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
		pipelineRenderingCreateInfo.sType						  = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfo.colorAttachmentCount		  = vkColourFormats.size();
		pipelineRenderingCreateInfo.pColorAttachmentFormats		  = vkColourFormats.data();
		pipelineRenderingCreateInfo.depthAttachmentFormat		  = depthStencilFormat;
		pipelineRenderingCreateInfo.stencilAttachmentFormat		  = depthStencilFormat;

		// create pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType						  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		pipelineInfo.pNext = nullptr;

		// enable dynamic rendering if requested and available
		if (deviceFeatures.DynamicRenderingAvailable)
		{
			pipelineInfo.pNext = &pipelineRenderingCreateInfo;
		}

		pipelineInfo.stageCount			 = shaderStages.size();
		pipelineInfo.pStages			 = shaderStages.data();
		pipelineInfo.pVertexInputState	 = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState		 = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState	 = &multisampleInfo;
		pipelineInfo.pColorBlendState	 = &colorBlending;
		pipelineInfo.pDepthStencilState	 = &depthStencilInfo;
		pipelineInfo.layout				 = pipelineLayout;

		VkPipelineDynamicStateCreateInfo dynamicInfo = {};
		dynamicInfo.sType							 = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicInfo.pNext							 = nullptr;
		dynamicInfo.flags							 = 0;

		std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
													 VK_DYNAMIC_STATE_SCISSOR,
													 VK_DYNAMIC_STATE_BLEND_CONSTANTS,
													 VK_DYNAMIC_STATE_STENCIL_REFERENCE};

		dynamicInfo.dynamicStateCount = dynamicStates.size();
		dynamicInfo.pDynamicStates	  = dynamicStates.data();

		pipelineInfo.pDynamicState = &dynamicInfo;
		pipelineInfo.renderPass	   = renderPass;

		pipelineInfo.subpass			= 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline pipeline = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}

		return pipeline;
	}

	VkResult AcquireNextImage(Graphics::GraphicsDeviceVk *device,
							  VkSwapchainKHR			  swapchain,
							  uint64_t					  timeout,
							  VkSemaphore				  semaphore,
							  VkFence					  fence,
							  uint32_t					 *imageIndex)
	{
		const Graphics::DeviceExtensionFunctions &functions = device->GetExtensionFunctions();
		if (functions.vkAcquireNextImage2KHR)
		{
			VkAcquireNextImageInfoKHR imageAcquireInfo = {};
			imageAcquireInfo.sType					   = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
			imageAcquireInfo.pNext					   = nullptr;
			imageAcquireInfo.swapchain				   = swapchain;
			imageAcquireInfo.timeout				   = timeout;
			imageAcquireInfo.semaphore				   = semaphore;
			imageAcquireInfo.fence					   = fence;
			imageAcquireInfo.deviceMask				   = 1;
			return vkAcquireNextImage2KHR(device->GetVkDevice(), &imageAcquireInfo, imageIndex);
		}
		else
		{
			return vkAcquireNextImageKHR(device->GetVkDevice(), swapchain, timeout, semaphore, fence, imageIndex);
		}
	}
}	 // namespace Nexus::Vk

#endif