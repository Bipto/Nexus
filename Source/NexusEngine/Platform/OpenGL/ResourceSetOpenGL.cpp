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

        OpenGLResourceInfo info{};
        info.Type = ResourceType::CombinedImageSampler;
        info.Resource = (TextureOpenGL *)texture;
        m_Resources[slot] = info;
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding)
    {
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(set, binding);

        OpenGLResourceInfo info{};
        info.Type = ResourceType::UniformBuffer;
        info.Resource = (UniformBufferOpenGL *)uniformBuffer;
        m_Resources[slot] = info;
    }

    const std::map<uint32_t, OpenGLResourceInfo> &ResourceSetOpenGL::GetResources() const
    {
        return m_Resources;
    }
}

#endif