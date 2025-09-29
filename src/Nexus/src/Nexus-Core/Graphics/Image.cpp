#include "Nexus-Core/Graphics/Image.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

#include "Nexus-Core/Graphics/CommandQueue.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
	void Image::FlipVertically()
	{
		Utils::FlipPixelsVertically(Pixels.data(), Width, Height, Format);
	}

	Image Image::FromTexture(GraphicsDevice	   *device,
							 Ref<ICommandQueue> commandQueue,
							 Ref<Texture>		texture,
							 uint32_t			arrayLayer,
							 uint32_t			mipLevel,
							 uint32_t			x,
							 uint32_t			y,
							 uint32_t			z,
							 uint32_t			width,
							 uint32_t			height)
	{
		std::vector<char> pixels = device->ReadFromTexture(texture, commandQueue, arrayLayer, mipLevel, x, y, z, width, height);

		Image image	 = {};
		image.Width	 = width;
		image.Height = height;
		image.Format = texture->GetDescription().Format;
		image.Pixels = pixels;

		return image;
	}
}	 // namespace Nexus::Graphics