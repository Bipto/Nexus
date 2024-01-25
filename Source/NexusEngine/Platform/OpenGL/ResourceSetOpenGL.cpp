#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec)
        : ResourceSet(spec)
    {
        // setup texture binding names
        for (const auto &textureBinding : m_Specification.TextureBindings)
        {
            uint32_t slot = ResourceSet::GetLinearDescriptorSlot(textureBinding.Set, textureBinding.Slot);
            m_TextureBindings[slot] = nullptr;
        }

        // setup uniform buffer binding names
        for (const auto &uniformBufferBinding : m_Specification.UniformResourceBindings)
        {
            uint32_t slot = ResourceSet::GetLinearDescriptorSlot(uniformBufferBinding.Set, uniformBufferBinding.Binding);
            m_UniformBufferBindings[slot] = nullptr;
        }
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteTexture(Texture *texture, uint32_t set, uint32_t binding)
    {
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);
        m_TextureBindings[slot] = (TextureOpenGL *)texture;
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding)
    {
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);
        m_UniformBufferBindings[slot] = (UniformBufferOpenGL *)uniformBuffer;
    }

    const std::map<uint32_t, TextureOpenGL *> &ResourceSetOpenGL::GetTextureBindings() const
    {
        return m_TextureBindings;
    }

    const std::map<uint32_t, UniformBufferOpenGL *> &ResourceSetOpenGL::GetUniformBufferBindings() const
    {
        return m_UniformBufferBindings;
    }
}

#endif