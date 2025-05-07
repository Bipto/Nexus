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
		Texture1D = 0,
		Texture2D = 1,
		Texture3D = 2
	};

	enum TextureUsageFlags : uint8_t
	{
		TextureUsage_Cubemap	  = BIT(0),
		TextureUsage_DepthStencil = BIT(1),
		TextureUsage_RenderTarget = BIT(2),
		TextureUsage_Sampled	  = BIT(3),
		TextureUsage_Storage	  = BIT(4)
	};

	struct TextureSpecification
	{
		PixelFormat Format		= PixelFormat::R8_G8_B8_A8_UNorm;
		uint32_t	Width		= 0;
		uint32_t	Height		= 0;
		uint32_t	Depth		= 0;
		uint32_t	MipLevels	= 1;
		uint32_t	ArrayLayers = 1;
		TextureType Type		= TextureType::Texture2D;
		uint32_t	Samples		= 1;
		uint8_t		Usage		= 0;
	};

	class NX_API Texture
	{
	  public:
		Texture(const TextureSpecification &spec) : m_Specification(spec)
		{
		}

		virtual ~Texture()
		{
		}

		const TextureSpecification &GetSpecification() const
		{
			return m_Specification;
		}

		bool IsDepth() const
		{
			if (m_Specification.Usage & Graphics::TextureUsage_DepthStencil)
			{
				return true;
			}

			return false;
		}

	  protected:
		TextureSpecification m_Specification = {};
	};

}	 // namespace Nexus::Graphics