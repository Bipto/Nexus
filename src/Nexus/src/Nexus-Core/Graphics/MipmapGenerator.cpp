#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Utils/Utils.hpp"
#include "stb_image_write.h"

namespace Nexus::Graphics
{
	std::vector<Image> MipmapGenerator::GenerateMipChain(Image baseImage)
	{
		std::vector<Image> images = {};

		return images;
	}

	std::vector<unsigned char> SampleNearestNeighbour(const Image &input, float uvX, float uvY)
	{
		uint32_t				   stride = GetPixelFormatSizeInBytes(input.Format);
		std::vector<unsigned char> pixel(stride);

		uvX = glm::clamp(uvX, 0.0f, 1.0f);
		uvY = glm::clamp(uvY, 0.0f, 1.0f);

		uint32_t pixelX = input.Width * uvX;
		uint32_t pixelY = input.Height * uvY;
		uint32_t offset = (pixelY * input.Width + pixelX) * stride;

		memcpy(pixel.data(), input.Pixels.data() + offset, stride);
		return pixel;
	}

	std::vector<unsigned char> SampleLinear(const Image &input, float uvX, float uvY)
	{
		uint32_t				   stride = GetPixelFormatSizeInBytes(input.Format);
		std::vector<unsigned char> pixel(stride, 0);

		// Clamp UV coordinates to [0, 1]
		uvX = glm::clamp(uvX, 0.0f, 1.0f) * input.Width;
		uvY = glm::clamp(uvY, 0.0f, 1.0f) * input.Height;

		// Determine integer and fractional parts
		int x0 = static_cast<int>(std::floor(uvX));
		int y0 = static_cast<int>(std::floor(uvY));
		int x1 = x0 + 1;
		int y1 = y0 + 1;

		float fracX = uvX - x0;
		float fracY = uvY - y0;

		// Clamp pixel indices to valid texture bounds
		x0 = glm::clamp(x0, 0, static_cast<int>(input.Width) - 1);
		y0 = glm::clamp(y0, 0, static_cast<int>(input.Height) - 1);
		x1 = glm::clamp(x1, 0, static_cast<int>(input.Width) - 1);
		y1 = glm::clamp(y1, 0, static_cast<int>(input.Height) - 1);

		// Offsets for the four surrounding pixels
		uint32_t offsetTL = (y0 * input.Width + x0) * stride;	 // Top-left
		uint32_t offsetTR = (y0 * input.Width + x1) * stride;	 // Top-right
		uint32_t offsetBL = (y1 * input.Width + x0) * stride;	 // Bottom-left
		uint32_t offsetBR = (y1 * input.Width + x1) * stride;	 // Bottom-right

		// Retrieve pixel colors and blend
		for (uint32_t i = 0; i < stride; i++)
		{
			float cTL = input.Pixels[offsetTL + i];	   // Top-left
			float cTR = input.Pixels[offsetTR + i];	   // Top-right
			float cBL = input.Pixels[offsetBL + i];	   // Bottom-left
			float cBR = input.Pixels[offsetBR + i];	   // Bottom-right

			// Bilinear interpolation
			float top	 = cTL + fracX * (cTR - cTL);									// Top row blend
			float bottom = cBL + fracX * (cBR - cBL);									// Bottom row blend
			pixel[i]	 = static_cast<unsigned char>(top + fracY * (bottom - top));	// Final blend
		}

		return pixel;
	}

	Image MipmapGenerator::GenerateMip(const Image &input, uint32_t level)
	{
		uint32_t		  stride = GetPixelFormatSizeInBytes(input.Format);

		Image output  = {};
		output.Format = input.Format;
		output.Width  = input.Width / 2;
		output.Height = input.Height / 2;
		output.Pixels.resize(output.Width * output.Height * stride);

		for (uint32_t y = 0; y < output.Height; y++)
		{
			for (uint32_t x = 0; x < output.Width; x++)
			{
				uint32_t index = (y * output.Width + x) * stride;

				float uvX = Utils::ReMapRange<float>(0, output.Width, 0, 1, x);
				float uvY = Utils::ReMapRange<float>(0, output.Height, 0, 1, y);

				std::vector<unsigned char> pixel = SampleLinear(input, uvX, uvY);
				memcpy(output.Pixels.data() + index, pixel.data(), stride);
			}
		}

		return output;
	}

	uint32_t MipmapGenerator::GetMaximumNumberOfMips(uint32_t width, uint32_t height)
	{
		return std::floor(std::log2(std::max(width, height)));
	}
}	 // namespace Nexus::Graphics