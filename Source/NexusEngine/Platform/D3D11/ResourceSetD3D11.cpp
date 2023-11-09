#include "ResourceSetD3D11.hpp"

#if defined(NX_PLATFORM_D3D11)

namespace Nexus::Graphics
{
    ResourceSetD3D11::ResourceSetD3D11(const ResourceSetSpecification &spec, GraphicsDeviceD3D11 *device)
        : ResourceSet(spec), m_GraphicsDevice(device)
    {
    }

    void ResourceSetD3D11::WriteTexture(Texture *texture, uint32_t binding)
    {
        m_TextureBindings[binding] = texture;
    }

    void ResourceSetD3D11::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding)
    {
        m_UniformBufferBindings[binding] = uniformBuffer;
    }

    const std::unordered_map<uint32_t, Texture *> &ResourceSetD3D11::GetTextureBindings()
    {
        return m_TextureBindings;
    }

    const std::unordered_map<uint32_t, UniformBuffer *> &ResourceSetD3D11::GetUniformBufferBindings()
    {
        return m_UniformBufferBindings;
    }
}

#endif