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

	enum TextureUsageFlags : uint8_t
	{
		TextureUsage_RenderTarget = BIT(0),
		TextureUsage_Sampled	  = BIT(1),
		TextureUsage_Storage	  = BIT(2)
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
		DepthReadOnlyStencilAttachmentOptimal,
		DepthAttachmentStencilReadOnlyOptimal,
		DepthAttachmentOptimal,
		DepthReadOnlyOptimal,
		StencilAttachmentOptimal,
		StencilReadOnlyOptimal,
		ReadonlyOptimal,
		PresentSrc
	};

	struct TextureDescription
	{
		TextureType Type			   = TextureType::Texture2D;
		PixelFormat Format			   = PixelFormat::R8_G8_B8_A8_UNorm;
		uint32_t	Width			   = 0;
		uint32_t	Height			   = 0;
		uint32_t	DepthOrArrayLayers = 1;
		uint32_t	MipLevels		   = 1;
		uint32_t	Samples			   = 1;
		uint8_t		Usage			   = 0;
		std::string DebugName		   = "Texture";
	};

	class NX_API Texture
	{
	  public:
		Texture(const TextureDescription &spec) : m_Description(spec)
		{
		}

		virtual ~Texture()
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

		virtual TextureLayout GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const = 0;

	  protected:
		TextureDescription m_Description = {};
	};

}	 // namespace Nexus::Graphics