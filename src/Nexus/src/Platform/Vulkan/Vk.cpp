#include "Vk.hpp"

#if defined(NX_PLATFORM_VULKAN)

namespace Nexus::Vk
{
	VkFormat GetVkPixelDataFormat(Nexus::Graphics::PixelFormat format, bool depthFormat)
	{
		switch (format)
		{
			case Nexus::Graphics::PixelFormat::R8_UNorm: return VK_FORMAT_R8_UNORM;
			case Nexus::Graphics::PixelFormat::R8_SNorm: return VK_FORMAT_R8_SNORM;
			case Nexus::Graphics::PixelFormat::R8_UInt: return VK_FORMAT_R8_UINT;
			case Nexus::Graphics::PixelFormat::R8_SInt: return VK_FORMAT_R8_SINT;

			case Nexus::Graphics::PixelFormat::R16_UNorm: return depthFormat ? VK_FORMAT_D16_UNORM : VK_FORMAT_R16_UNORM;
			case Nexus::Graphics::PixelFormat::R16_SNorm: return VK_FORMAT_R16_SNORM;
			case Nexus::Graphics::PixelFormat::R16_UInt: return VK_FORMAT_R16_UINT;
			case Nexus::Graphics::PixelFormat::R16_SInt: return VK_FORMAT_R16_SINT;
			case Nexus::Graphics::PixelFormat::R16_Float: return VK_FORMAT_R16_SFLOAT;

			case Nexus::Graphics::PixelFormat::R32_UInt: return VK_FORMAT_R32_UINT;
			case Nexus::Graphics::PixelFormat::R32_SInt: return VK_FORMAT_R32_SINT;
			case Nexus::Graphics::PixelFormat::R32_Float: return depthFormat ? VK_FORMAT_D32_SFLOAT : VK_FORMAT_R32_SFLOAT;

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

			case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;
			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
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

			case VK_FORMAT_D32_SFLOAT_S8_UINT: return Nexus::Graphics::PixelFormat::D32_Float_S8_UInt;
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
			case Nexus::Graphics::ShaderDataType::Byte: return VK_FORMAT_R8_UINT; break;
			case Nexus::Graphics::ShaderDataType::Byte2: return VK_FORMAT_R8G8_UINT; break;
			case Nexus::Graphics::ShaderDataType::Byte4: return VK_FORMAT_R8G8B8A8_UINT; break;

			case Nexus::Graphics::ShaderDataType::NormByte: return VK_FORMAT_R8_UNORM; break;
			case Nexus::Graphics::ShaderDataType::NormByte2: return VK_FORMAT_R8G8_UNORM; break;
			case Nexus::Graphics::ShaderDataType::NormByte4: return VK_FORMAT_R8G8B8A8_UNORM; break;

			case Nexus::Graphics::ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT; break;

			case Nexus::Graphics::ShaderDataType::Half: return VK_FORMAT_R16_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::Half2: return VK_FORMAT_R16G16_SFLOAT; break;
			case Nexus::Graphics::ShaderDataType::Half4: return VK_FORMAT_R16G16B16A16_SFLOAT; break;

			case Nexus::Graphics::ShaderDataType::Int: return VK_FORMAT_R32_SINT; break;
			case Nexus::Graphics::ShaderDataType::Int2: return VK_FORMAT_R32G32_SINT; break;
			case Nexus::Graphics::ShaderDataType::Int3: return VK_FORMAT_R32G32B32_SINT; break;
			case Nexus::Graphics::ShaderDataType::Int4: return VK_FORMAT_R32G32B32A32_SINT; break;

			case Nexus::Graphics::ShaderDataType::SignedByte: return VK_FORMAT_R8_SINT; break;
			case Nexus::Graphics::ShaderDataType::SignedByte2: return VK_FORMAT_R8G8_SINT; break;
			case Nexus::Graphics::ShaderDataType::SignedByte4: return VK_FORMAT_R8G8B8A8_SINT; break;

			case Nexus::Graphics::ShaderDataType::SignedByteNormalized: return VK_FORMAT_R8_SNORM; break;
			case Nexus::Graphics::ShaderDataType::SignedByte2Normalized: return VK_FORMAT_R8G8_SNORM; break;
			case Nexus::Graphics::ShaderDataType::SignedByte4Normalized: return VK_FORMAT_R8G8B8A8_SNORM; break;

			case Nexus::Graphics::ShaderDataType::Short: return VK_FORMAT_R16_SINT; break;
			case Nexus::Graphics::ShaderDataType::Short2: return VK_FORMAT_R16G16_SINT; break;
			case Nexus::Graphics::ShaderDataType::Short4: return VK_FORMAT_R16G16B16A16_SINT; break;

			case Nexus::Graphics::ShaderDataType::ShortNormalized: return VK_FORMAT_R16_SNORM; break;
			case Nexus::Graphics::ShaderDataType::Short2Normalized: return VK_FORMAT_R16G16_SNORM; break;
			case Nexus::Graphics::ShaderDataType::Short4Normalized: return VK_FORMAT_R16G16B16A16_SNORM; break;

			case Nexus::Graphics::ShaderDataType::UInt: return VK_FORMAT_R32_UINT; break;
			case Nexus::Graphics::ShaderDataType::UInt2: return VK_FORMAT_R32G32_UINT; break;
			case Nexus::Graphics::ShaderDataType::UInt3: return VK_FORMAT_R32G32B32_UINT; break;
			case Nexus::Graphics::ShaderDataType::UInt4: return VK_FORMAT_R32G32B32A32_UINT; break;

			case Nexus::Graphics::ShaderDataType::UShort: return VK_FORMAT_R16_UINT; break;
			case Nexus::Graphics::ShaderDataType::UShort2: return VK_FORMAT_R16G16_UINT; break;
			case Nexus::Graphics::ShaderDataType::UShort4: return VK_FORMAT_R16G16B16A16_UINT; break;

			case Nexus::Graphics::ShaderDataType::UShortNormalized: return VK_FORMAT_R16_UNORM; break;
			case Nexus::Graphics::ShaderDataType::UShort2Normalized: return VK_FORMAT_R16G16B16_UNORM; break;
			case Nexus::Graphics::ShaderDataType::UShort4Normalized: return VK_FORMAT_R16G16B16A16_UNORM; break;
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

	VkBlendFactor GetVkBlendFactor(Nexus::Graphics::BlendFactor function)
	{
		switch (function)
		{
			case Nexus::Graphics::BlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
			case Nexus::Graphics::BlendFactor::One: return VK_BLEND_FACTOR_ONE;
			case Nexus::Graphics::BlendFactor::SourceColor: return VK_BLEND_FACTOR_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusSourceColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::DestinationColor: return VK_BLEND_FACTOR_DST_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case Nexus::Graphics::BlendFactor::SourceAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::DestinationAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
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

	VkImageUsageFlagBits GetVkImageUsageFlags(const std::vector<Nexus::Graphics::TextureUsage> &usage, bool &isDepth)
	{
		isDepth					   = false;
		VkImageUsageFlagBits flags = VkImageUsageFlagBits(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

		for (const auto &item : usage)
		{
			if (item == Nexus::Graphics::TextureUsage::DepthStencil)
			{
				isDepth = true;
			}
		}

		for (const auto &item : usage)
		{
			if (item == Nexus::Graphics::TextureUsage::DepthStencil)
			{
				flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_SAMPLED_BIT);
			}

			if (item == Nexus::Graphics::TextureUsage::RenderTarget)
			{
				if (isDepth)
				{
					flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
				}
				else
				{
					flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
				}
			}

			if (item == Nexus::Graphics::TextureUsage::Sampled)
			{
				flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_SAMPLED_BIT);
			}

			if (item == Nexus::Graphics::TextureUsage::Storage)
			{
				flags = VkImageUsageFlagBits(flags | VK_IMAGE_USAGE_STORAGE_BIT);
			}
		}

		return flags;
	}

	VkShaderStageFlagBits GetVkShaderStageFlags(Nexus::Graphics::ShaderStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case Nexus::Graphics::ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
			case Nexus::Graphics::ShaderStage::TesselationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case Nexus::Graphics::ShaderStage::TesselationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case Nexus::Graphics::ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			case Nexus::Graphics::ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}

	VkIndexType GetVulkanIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::IndexBufferFormat::UInt16: return VK_INDEX_TYPE_UINT16;
			case Nexus::Graphics::IndexBufferFormat::UInt32: return VK_INDEX_TYPE_UINT32;
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

	VkBufferUsageFlags GetVkBufferUsage(const Graphics::DeviceBufferDescription &desc)
	{
		switch (desc.Type)
		{
			case Graphics::DeviceBufferType::Vertex:
				return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case Graphics::DeviceBufferType::Index:
				return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case Graphics::DeviceBufferType::Uniform:
				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case Graphics::DeviceBufferType::Structured:
				return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case Graphics::DeviceBufferType::Upload: return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case Graphics::DeviceBufferType::Readback: return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case Graphics::DeviceBufferType::Indirect:
				return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			default: throw std::runtime_error("Failed to find a valid VkBufferUsage");
		}
	}

	VkBufferCreateInfo GetVkBufferCreateInfo(const Graphics::DeviceBufferDescription &desc)
	{
		VkBufferUsageFlags bufferUsage = GetVkBufferUsage(desc);

		VkBufferCreateInfo createInfo = {};
		createInfo.sType			  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size				  = desc.SizeInBytes;
		createInfo.usage			  = bufferUsage;
		return createInfo;
	}

	VmaAllocationCreateInfo GetVmaAllocationCreateInfo(const Graphics::DeviceBufferDescription &desc)
	{
		VmaAllocationCreateInfo createInfo = {};
		createInfo.usage				   = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		if (desc.Type == Graphics::DeviceBufferType::Upload || desc.Type == Graphics::DeviceBufferType::Readback || desc.HostVisible == true)
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

	VkPipelineStageFlags2 GetBarrierPipelineStage(Nexus::Graphics::BarrierStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::BarrierStage::None: return VK_PIPELINE_STAGE_NONE;
			case Nexus::Graphics::BarrierStage::All: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			case Nexus::Graphics::BarrierStage::Graphics: return VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
			case Nexus::Graphics::BarrierStage::VertexInput: return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
			case Nexus::Graphics::BarrierStage::VertexShader: return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			case Nexus::Graphics::BarrierStage::FragmentShader: return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			case Nexus::Graphics::BarrierStage::TesselationControlShader: return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
			case Nexus::Graphics::BarrierStage::TesselationEvaluationShader: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			case Nexus::Graphics::BarrierStage::GeometryShader: return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			case Nexus::Graphics::BarrierStage::ComputeShader: return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			case Nexus::Graphics::BarrierStage::RenderTarget: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			case Nexus::Graphics::BarrierStage::TransferSource: return VK_PIPELINE_STAGE_TRANSFER_BIT;
			case Nexus::Graphics::BarrierStage::TransferDestination: return VK_PIPELINE_STAGE_TRANSFER_BIT;
			case Nexus::Graphics::BarrierStage::Resolve: return VK_PIPELINE_STAGE_2_RESOLVE_BIT;
			default: throw std::runtime_error("Failed to find a valid pipeline stage");
		}
	}

	VkAccessFlags2 GetBarrierAccessFlags(Nexus::Graphics::BarrierAccess access)
	{
		switch (access)
		{
			case Nexus::Graphics::BarrierAccess::None: return VK_ACCESS_NONE;
			case Nexus::Graphics::BarrierAccess::All: return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
			case Nexus::Graphics::BarrierAccess::VertexBuffer: return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
			case Nexus::Graphics::BarrierAccess::IndexBuffer: return VK_ACCESS_INDEX_READ_BIT;
			case Nexus::Graphics::BarrierAccess::RenderTarget: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			case Nexus::Graphics::BarrierAccess::DepthStencilRead: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			case Nexus::Graphics::BarrierAccess::DepthStencilWrite:
				return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			case Nexus::Graphics::BarrierAccess::ResolveSource: return VK_ACCESS_MEMORY_READ_BIT;
			case Nexus::Graphics::BarrierAccess::ResolveDestination: return VK_ACCESS_MEMORY_WRITE_BIT;
			case Nexus::Graphics::BarrierAccess::CopySource: return VK_ACCESS_MEMORY_READ_BIT;
			case Nexus::Graphics::BarrierAccess::CopyDestination: return VK_ACCESS_MEMORY_WRITE_BIT;
			case Nexus::Graphics::BarrierAccess::DrawIndirect: return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
			default: throw std::runtime_error("Failed to find a valid barrier access type");
		}
	}

	VkImageLayout GetBarrierLayout(Nexus::Graphics::BarrierLayout layout)
	{
		switch (layout)
		{
			case Nexus::Graphics::BarrierLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
			case Nexus::Graphics::BarrierLayout::General: return VK_IMAGE_LAYOUT_GENERAL;
			case Nexus::Graphics::BarrierLayout::Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case Nexus::Graphics::BarrierLayout::RenderTarget: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::DepthStencilRead: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::DepthStencilWrite: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::CopySource: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::CopyDestination: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::ResolveSource: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::ResolveDestimation: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::ShaderReadOnly: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case Nexus::Graphics::BarrierLayout::ShaderReadWrite: return VK_IMAGE_LAYOUT_GENERAL;
			default: throw std::runtime_error("Failed to find a valid barrier layout");
		}
	}
}	 // namespace Nexus::Vk

#endif