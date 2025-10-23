#pragma once

#include "Nexus-Core/Graphics/Image.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "PixelFormat.hpp"
#include "SamplerState.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	enum class TextureType
	{
		Texture1D	= 0,
		Texture2D	= 1,
		Texture3D	= 2,
		TextureCube = 3
	};

	enum TextureUsageFlags : uint16_t
	{
		TextureUsage_None					= 0,
		TextureUsage_TransferSrc			= BIT(0),
		TextureUsage_TransferDst			= BIT(1),
		TextureUsage_Sampled				= BIT(2),
		TextureUsage_Storage				= BIT(3),
		TextureUsage_ColourAttachment		= BIT(4),
		TextureUsage_DepthStencilAttachment = BIT(5),
		TextureUsage_VideoDecodeDst			= BIT(6),
		TextureUsage_VideoEncodeSrc			= BIT(7),
	};

	enum TextureCreateFlags : uint8_t
	{
		TextureCreateFlags_None			 = 0,
		TextureCreateFlags_SparseBinding = BIT(0)
	};

	enum class TextureLayout
	{
		Undefined,
		General,
		ColourAttachmentOptimal,
		DepthStencilAttachmentOptimal,
		DepthStencilReadOnlyOptimal,
		ShaderReadOnlyOptimal,
		TransferSrcOptimal,
		TransferDstOptimal,
		PresentSrc,
		VideoEncodeDestination,
		VideoEncodeSource,
		VideoDecodeDestination,
		VideoDecodeSource
	};

	struct TextureDescription
	{
		TextureType Type			   = TextureType::Texture2D;
		uint8_t		CreateFlags		   = TextureCreateFlags_None;
		PixelFormat Format			   = PixelFormat::R8_G8_B8_A8_UNorm;
		uint32_t	Width			   = 0;
		uint32_t	Height			   = 0;
		uint32_t	DepthOrArrayLayers = 1;
		uint32_t	MipLevels		   = 1;
		uint32_t	Samples			   = 1;
		uint16_t	Usage			   = TextureUsage_None;
		std::string DebugName		   = "Texture";
	};

	class NX_API ITexture
	{
	  public:
		ITexture(const TextureDescription &spec) : m_Description(spec)
		{
		}

		virtual ~ITexture()
		{
		}

		const TextureDescription &GetDescription() const
		{
			return m_Description;
		}

		bool IsColour() const
		{
			return GetPixelFormatType(m_Description.Format) == PixelFormatType::Colour;
		}

		bool IsDepth() const
		{
			return GetPixelFormatType(m_Description.Format) == PixelFormatType::DepthStencil;
		}

		PixelFormat GetPixelFormat() const
		{
			return m_Description.Format;
		}

		uint32_t GetSampleCount() const
		{
			return m_Description.Samples;
		}

		uint32_t GetWidth() const
		{
			return m_Description.Width;
		}

		uint32_t GetHeight() const
		{
			return m_Description.Height;
		}

		uint16_t GetUsage() const
		{
			return m_Description.Usage;
		}

		TextureType GetType() const
		{
			return m_Description.Type;
		}

		uint32_t GetDepthOrArrayLayers() const
		{
			return m_Description.DepthOrArrayLayers;
		}

		uint32_t GetMipLevels() const
		{
			return m_Description.MipLevels;
		}

		virtual TextureLayout GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const = 0;

	  protected:
		TextureDescription m_Description = {};
	};

}	 // namespace Nexus::Graphics