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
        m_BoundUniformBuffers[name] = std::dynamic_pointer_cast<UniformBufferOpenGL>(uniformBuffer);
    }

    void ResourceSetOpenGL::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
    {
        m_BoundTextures[name] = std::dynamic_pointer_cast<TextureOpenGL>(texture);
        m_BoundSamplers[name] = std::dynamic_pointer_cast<SamplerOpenGL>(sampler);
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