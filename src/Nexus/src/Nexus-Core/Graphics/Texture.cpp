#include "Texture.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
Texture::Texture(const TextureSpecification &spec, GraphicsDevice *device) : m_Specification(spec), m_Device(device)
{
}

const TextureSpecification &Texture::GetTextureSpecification()
{
    return m_Specification;
}

uint32_t Texture::GetLevels() const
{
    return m_Specification.Levels;
}

} // namespace Nexus::Graphics
