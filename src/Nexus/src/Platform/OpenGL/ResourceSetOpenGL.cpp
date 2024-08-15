#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec) : ResourceSet(spec)
{
}

void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name)
{
    m_BoundUniformBuffers[name] = std::dynamic_pointer_cast<UniformBufferOpenGL>(uniformBuffer);
}

void ResourceSetOpenGL::WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name)
{
    CombinedImageSampler ciSampler{};
    ciSampler.ImageTexture = texture;
    ciSampler.ImageSampler = sampler;
    m_BoundCombinedImageSamplers[name] = ciSampler;
}
} // namespace Nexus::Graphics

#endif