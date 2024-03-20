#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec)
        : ResourceSet(spec)
    {
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name)
    {
        m_CachedUniformBuffers[name] = std::dynamic_pointer_cast<UniformBufferOpenGL>(uniformBuffer);
    }

    void ResourceSetOpenGL::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
    {
        m_CachedTextures[name] = std::dynamic_pointer_cast<TextureOpenGL>(texture);
        m_CachedSamplers[name] = std::dynamic_pointer_cast<SamplerOpenGL>(sampler);
    }

    void ResourceSetOpenGL::Flush()
    {
        m_BoundTextures = m_CachedTextures;
        m_BoundSamplers = m_CachedSamplers;
        m_BoundUniformBuffers = m_CachedUniformBuffers;
    }

    const std::map<std::string, Ref<TextureOpenGL>> &ResourceSetOpenGL::GetBoundTextures() const
    {
        return m_BoundTextures;
    }

    const std::map<std::string, Ref<UniformBufferOpenGL>> &ResourceSetOpenGL::GetBoundUniformBuffers() const
    {
        return m_BoundUniformBuffers;
    }

    const std::map<std::string, Ref<SamplerOpenGL>> &ResourceSetOpenGL::GetBoundSamplers() const
    {
        return m_BoundSamplers;
    }
}

#endif