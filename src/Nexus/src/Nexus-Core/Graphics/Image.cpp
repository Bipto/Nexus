#include "Nexus-Core/Graphics/Image.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	void Image::FlipVertically()
	{
		Utils::FlipPixelsVertically(Pixels, Width, Height, Format);
	}
}	 // namespace Nexus::Graphics
