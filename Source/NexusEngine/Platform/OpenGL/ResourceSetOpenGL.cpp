#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec)
        : ResourceSet(spec)
    {
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteTexture(Texture *texture, const std::string &name)
    {
        // const uint32_t index = GetLinearDescriptorSlot(set, binding);
        // m_BoundTextures[index] = (TextureOpenGL *)texture;

        m_BoundTextures[name] = (TextureOpenGL *)texture;
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name)
    {
        // const uint32_t index = GetLinearDescriptorSlot(set, binding);
        // m_BoundUniformBuffers[index] = (UniformBufferOpenGL *)uniformBuffer;

        m_BoundUniformBuffers[name] = (UniformBufferOpenGL *)uniformBuffer;
    }

    const std::map<std::string, TextureOpenGL *> &ResourceSetOpenGL::GetBoundTextures() const
    {
        return m_BoundTextures;
    }

    const std::map<std::string, UniformBufferOpenGL *> &ResourceSetOpenGL::GetBoundUniformBuffers() const
    {
        return m_BoundUniformBuffers;
    }
}

#endif