#include "ResourceSetDX11.hpp"

namespace Nexus::Graphics
{
    ResourceSetDX11::ResourceSetDX11(const ResourceSetSpecification &spec, GraphicsDeviceDX11 *device)
        : ResourceSet(spec), m_GraphicsDevice(device)
    {
    }

    void ResourceSetDX11::WriteTexture(Ref<Texture> texture, uint32_t binding)
    {
        m_TextureBindings[binding] = texture;
    }

    void ResourceSetDX11::WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding)
    {
        m_UniformBufferBindings[binding] = uniformBuffer;
    }

    const std::unordered_map<uint32_t, Ref<Texture>> &ResourceSetDX11::GetTextureBindings()
    {
        return m_TextureBindings;
    }

    const std::unordered_map<uint32_t, Ref<UniformBuffer>> &ResourceSetDX11::GetUniformBufferBindings()
    {
        return m_UniformBufferBindings;
    }
}
