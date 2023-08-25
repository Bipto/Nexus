#include "TextureVk.hpp"

namespace Nexus::Graphics
{
    TextureVk::TextureVk(GraphicsDeviceVk *graphicsDevice, const TextureSpecification &spec)
        : Texture(spec)
    {
    }

    TextureVk::~TextureVk()
    {
    }

    void *TextureVk::GetHandle()
    {
        return nullptr;
    }
}