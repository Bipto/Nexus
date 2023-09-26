#include "ResourceSetDX11.hpp"

#if defined(NX_PLATFORM_DX11)

namespace Nexus::Graphics
{
    ResourceSetDX11::ResourceSetDX11(const ResourceSetSpecification &spec, GraphicsDeviceDX11 *device)
        : ResourceSet(spec), m_GraphicsDevice(device)
    {
    }

    void ResourceSetDX11::WriteTexture(Texture *texture, uint32_t binding)
    {
        m_TextureBindings[binding] = texture;
    }

    void ResourceSetDX11::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding)
    {
        m_UniformBufferBindings[binding] = uniformBuffer;
    }

    const std::unordered_map<uint32_t, Texture *> &ResourceSetDX11::GetTextureBindings()
    {
        return m_TextureBindings;
    }

    const std::unordered_map<uint32_t, UniformBuffer *> &ResourceSetDX11::GetUniformBufferBindings()
    {
        return m_UniformBufferBindings;
    }
}

#endif