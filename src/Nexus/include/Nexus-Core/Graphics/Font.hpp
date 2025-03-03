#pragma once

#include "GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Color.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Texture.hpp"

namespace Nexus::Graphics
{
	struct CharacterRange
	{
		uint32_t Begin;
		uint32_t End;
	};

	struct CharacterTextureCoords
	{
		Nexus::Point2D<float> Min;
		Nexus::Point2D<float> Max;
	};

	struct FontData
	{
	  public:
		FontData() = delete;
		FontData(uint32_t width, uint32_t height)
		{
			m_Width	 = width;
			m_Height = height;
			m_Pixels.resize(m_Width * m_Height);
		}

		std::vector<uint8_t> &GetPixels()
		{
			return m_Pixels;
		}

		void Clear(unsigned char value)
		{
			for (int i = 0; i < m_Pixels.size(); i++) { m_Pixels[i] = value; }
		}

		void SetPixel(uint32_t x, uint32_t y, unsigned char value)
		{
			x %= m_Width;
			y %= m_Height;

			y = m_Height - y - 1;

			uint32_t offset = x + (y * m_Width);

			// Color color = Nexus::Graphics::Color(255, 255, 255, 255);
			// m_Pixels[offset] = color.GetColor();

			m_Pixels[offset] = value;
		}

		uint32_t GetWidth() const
		{
			return m_Width;
		}

		uint32_t GetHeight() const
		{
			return m_Height;
		}

	  private:
		std::vector<uint8_t> m_Pixels;
		uint32_t			 m_Width  = 0;
		uint32_t			 m_Height = 0;
	};

	struct Character
	{
		glm::vec2 Size;
		glm::vec2 Bearing;
		uint32_t  Advance;
		glm::vec2 TexCoordsMin;
		glm::vec2 TexCoordsMax;
	};

	enum class FontType
	{
		SDF,
		Bitmap
	};

	class NX_API Font
	{
	  public:
		Font(const std::string &filepath, uint32_t size, const std::vector<CharacterRange> &characterRanges, FontType type, GraphicsDevice *device);

		Nexus::Ref<Nexus::Graphics::Texture2D> GetTexture();
		const Character						  &GetCharacter(char character);
		uint32_t							   GetSize() const;
		Nexus::Point2D<float>				   MeasureString(const std::string &text, uint32_t size);
		const uint32_t						   GetLineHeight() const;
		const Point2D<uint32_t>				   GetMaxCharacterSize() const;
		const FontType						   GetFontType() const;

	  private:
		Nexus::Ref<Nexus::Graphics::Texture2D> m_Texture = nullptr;
		std::vector<CharacterRange>			   m_CharacterRanges;
		std::map<char, Character>			   m_Characters;
		uint32_t							   m_TextureWidth	   = 0;
		uint32_t							   m_TextureHeight	   = 0;
		uint32_t							   m_FontSize		   = 96;
		uint32_t							   m_LineSpacing	   = 0;
		uint32_t							   m_UnderlinePosition = 0;
		Point2D<uint32_t>					   m_MaxCharacterSize  = {0, 0};
		FontType							   m_Type;
	};
}	 // namespace Nexus::Graphics