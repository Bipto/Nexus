#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct PixelR8G8B8A8
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	class PixelBuffer
	{
	  public:
		PixelBuffer() = default;

		PixelBuffer(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
		{
			m_Pixels.resize(m_Width * m_Height * 4);
		}

		~PixelBuffer()
		{
		}

		void Resize(uint32_t width, uint32_t height)
		{
			m_Width	 = width;
			m_Height = height;
			m_Pixels.resize(m_Width * m_Height * 4);
		}

		void SetPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		{
			size_t index		= (y * m_Width + x) * 4;
			m_Pixels[index + 0] = r;
			m_Pixels[index + 1] = g;
			m_Pixels[index + 2] = b;
			m_Pixels[index + 3] = a;
		}

		PixelR8G8B8A8 GetPixel(uint32_t x, uint32_t y)
		{
			size_t index = (y * m_Width + x) * 4;

			PixelR8G8B8A8 pixel = {};
			pixel.r				= m_Pixels[index + 0];
			pixel.g				= m_Pixels[index + 1];
			pixel.b				= m_Pixels[index + 2];
			pixel.a				= m_Pixels[index + 3];

			return pixel;
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
		uint32_t			 m_Width  = 0;
		uint32_t			 m_Height = 0;
		std::vector<uint8_t> m_Pixels = {};
	};
}	 // namespace Nexus::Graphics