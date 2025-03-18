#include "Nexus-Core/Assets/Processors/TextureProcessor.hpp"

#include "Nexus-Core/Graphics/Image.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

namespace Nexus::Processors
{
	TextureProcessor::TextureProcessor()
	{
	}

	TextureProcessor::~TextureProcessor()
	{
	}

	GUID TextureProcessor::Process(const std::string &filepath, Graphics::GraphicsDevice *device)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *pixels = (unsigned char *)stbi_load(filepath.c_str(), &width, &height, &channels, 4);

		uint32_t stride	  = 4;
		uint32_t dataSize = width * height * stride;

		Graphics::Image image = {};
		image.Format		  = Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		image.Width			  = width;
		image.Height		  = height;
		image.Pixels.resize(dataSize);
		memcpy(image.Pixels.data(), pixels, image.Pixels.size());
		stbi_image_free(pixels);

		Graphics::MipmapGenerator generator = {};
		Graphics::Image			  mip		= generator.GenerateMip(image, 1);
		stbi_write_png("test.png", mip.Width, mip.Height, 4, mip.Pixels.data(), mip.Width * 4);

		return GUID();
	}
}	 // namespace Nexus::Processors