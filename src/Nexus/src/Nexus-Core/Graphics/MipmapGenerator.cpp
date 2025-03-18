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

	const unsigned char *SamplePixel(const Image &input, float uvX, float uvY)
	{
		uint32_t stride = GetPixelFormatSizeInBytes(input.Format);

		uint32_t pixelX = input.Width * uvX;
		uint32_t pixelY = input.Height * uvY;
		uint32_t offset = (pixelY * input.Width + pixelX) * stride;

		return input.Pixels.data() + offset;
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
				uint32_t flippedY = output.Height - 1 - y;
				uint32_t index = (y * output.Width + x) * stride;

				float uvX = Utils::ReMapRange<float>(0, output.Width, 0, 1, x);
				float uvY = Utils::ReMapRange<float>(0, output.Height, 0, 1, y);

				const unsigned char *pixel = SamplePixel(input, uvX, uvY);
				memcpy(output.Pixels.data() + index, pixel, stride);
			}
		}

		return output;
	}

	uint32_t MipmapGenerator::GetMaximumNumberOfMips(uint32_t width, uint32_t height)
	{
		return std::floor(std::log2(std::max(width, height)));
	}
}	 // namespace Nexus::Graphics