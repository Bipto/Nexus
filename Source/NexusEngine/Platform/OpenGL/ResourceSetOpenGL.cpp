#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec)
        : ResourceSet(spec)
    {
    }

    void ResourceSetOpenGL::PerformResourceUpdate()
    {
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteTexture(Texture *texture, const std::string &name)
    {
        m_BoundTextures[name] = (TextureOpenGL *)texture;
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name)
    {
        m_BoundUniformBuffers[name] = (UniformBufferOpenGL *)uniformBuffer;
    }

    void ResourceSetOpenGL::WriteSampler(Sampler *sampler, const std::string &name)
    {
        m_BoundSamplers[name] = (SamplerOpenGL *)sampler;
    }

    void ResourceSetOpenGL::WriteCombinedImageSampler(Texture *texture, Sampler *sampler, const std::string &name)
    {
        WriteTexture(texture, name);
        WriteSampler(sampler, name);
    }

    const std::map<std::string, TextureOpenGL *> &ResourceSetOpenGL::GetBoundTextures() const
    {
        return m_BoundTextures;
    }

    const std::map<std::string, UniformBufferOpenGL *> &ResourceSetOpenGL::GetBoundUniformBuffers() const
    {
        return m_BoundUniformBuffers;
    }

    const std::map<std::string, SamplerOpenGL *> &ResourceSetOpenGL::GetBoundSamplers() const
    {
        return m_BoundSamplers;
    }
}

#endif