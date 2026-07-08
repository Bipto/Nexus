#include "TextureViewD3D12.hpp"

namespace Nexus::Graphics
{
    TextureViewD3D12::TextureViewD3D12(const TextureViewDescription &desc) : m_Description(desc)
    {
    }

    TextureViewD3D12::~TextureViewD3D12()
    {
    }

    const TextureViewDescription &TextureViewD3D12::GetDescription() const
    {
        return m_Description;
    }
} // namespace Nexus::Graphics