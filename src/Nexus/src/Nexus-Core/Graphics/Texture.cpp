#include "Texture.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
    Texture::Texture(const TextureSpecification &spec, GraphicsDevice *device)
        : m_Specification(spec), m_Device(device)
    {
        InitialiseLayouts();
    }

    const TextureSpecification &Texture::GetTextureSpecification()
    {
        return m_Specification;
    }

    uint32_t Texture::GetLevels() const
    {
        return m_Specification.Levels;
    }

    std::optional<ImageLayout> Texture::GetImageLayout(uint32_t level)
    {
        if (level > m_Specification.Levels)
        {
            NX_ERROR("Attempting to read the layout of an invalid mip level");
            return {};
        }

        return m_Layouts.at(level);
    }

    void Texture::InitialiseLayouts()
    {
        for (uint32_t i = 0; i < m_Specification.Levels; i++)
        {
            m_Layouts[i] = ImageLayout::General;
        }
    }

    void Texture::SetImageLayout(uint32_t level, ImageLayout layout)
    {
        if (level > m_Specification.Levels)
        {
            NX_ERROR("Attempting to set the layout of an invalid mip level");
        }

        m_Layouts[level] = layout;
    }
}
