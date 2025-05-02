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
		uint32_t	MipLevels	= 0;
		uint32_t	ArrayLayers = 0;
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

	  protected:
		TextureSpecification m_Specification = {};
	};

	enum class TextureUsage : uint8_t
	{
		DepthStencil,
		RenderTarget,
		Sampled,
		Storage
	};

	/// @brief A struct that represents a set of values to use to create a texture
	struct Texture2DSpecification
	{
		/// @brief An unsigned 32 bit integer representing the width of the texture
		uint32_t Width = 512;

		/// @brief An unsigned 32 bit integer representing the height of the texture
		uint32_t Height = 512;

		/// @brief An unsigned 32 bit integer representing the number of mips in the
		/// texture
		uint32_t MipLevels = 1;

		/// @brief An unsigned 32 bit integer representing how many array layers to
		/// store in the texture
		uint32_t ArrayLayers = 1;

		/// @brief A value representing how many bits will be allocated for the
		/// texture
		PixelFormat Format = PixelFormat::R8_G8_B8_A8_UNorm;

		/// @brief A set of values to use for creating the texture
		std::vector<TextureUsage> Usage = {TextureUsage::Sampled};

		/// @brief How many samples should be used to sample a pixel
		uint32_t Samples = 1;
	};

	struct CubemapSpecification
	{
		uint32_t	Width	  = 512;
		uint32_t	Height	  = 512;
		uint32_t	MipLevels = 1;
		PixelFormat Format	  = PixelFormat::R8_G8_B8_A8_UNorm;
	};

	class GraphicsDevice;

	/// @brief A pure virtual class that represents an API specific texture
	class NX_API Texture2D
	{
	  public:
		/// @brief A texture cannot be created without a texture specification
		Texture2D() = delete;

		/// @brief A constructor that takes in a texture specification
		/// @param spec The specification to create a texture from
		Texture2D(const Texture2DSpecification &spec, GraphicsDevice *device);

		virtual ~Texture2D();

		// virtual void SetData(const void *data, uint32_t size, uint32_t level) = 0;
		virtual void SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void GetData(std::vector<unsigned char> &pixels, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		std::vector<unsigned char> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		Image					   GetDataAsImage(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		Image					   GetDataAsImage(uint32_t level);

		const Texture2DSpecification &GetSpecification();
		uint32_t					  GetLevels() const;

	  protected:
		/// @brief A specification describing the layout of the texture
		Texture2DSpecification m_Specification {};

	  private:
		GraphicsDevice *m_Device = nullptr;
	};

	enum class CubemapFace
	{
		PositiveX = 0,
		NegativeX = 1,
		NegativeY = 2,
		PositiveY = 3,
		PositiveZ = 4,
		NegativeZ = 5
	};

	class NX_API Cubemap
	{
	  public:
		Cubemap() = delete;
		Cubemap(const CubemapSpecification &spec, GraphicsDevice *device);
		virtual ~Cubemap();

		virtual void SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		std::vector<unsigned char> GetData(CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		virtual void			   GetData(std::vector<unsigned char> &pixels,
										   CubemapFace				   face,
										   uint32_t					   level,
										   uint32_t					   x,
										   uint32_t					   y,
										   uint32_t					   width,
										   uint32_t					   height) = 0;

		const CubemapSpecification &GetSpecification() const;
		uint32_t					GetLevels() const;

	  protected:
		CubemapSpecification m_Specification {};

	  private:
		GraphicsDevice *m_Device = nullptr;
	};

}	 // namespace Nexus::Graphics