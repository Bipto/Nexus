#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec)
        : ResourceSet(spec)
    {
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteTexture(Texture *texture, uint32_t set, uint32_t binding)
    {
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);
        m_BoundTextures[slot] = (TextureOpenGL *)texture;
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding)
    {
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);
        m_BoundUniformBuffers[slot] = (UniformBufferOpenGL *)uniformBuffer;
    }

    const std::map<uint32_t, TextureOpenGL *> &ResourceSetOpenGL::GetBoundTextures() const
    {
        return m_BoundTextures;
    }

    const std::map<uint32_t, UniformBufferOpenGL *> &ResourceSetOpenGL::GetBoundUniformBuffers() const
    {
        return m_BoundUniformBuffers;
    }
}

#endif