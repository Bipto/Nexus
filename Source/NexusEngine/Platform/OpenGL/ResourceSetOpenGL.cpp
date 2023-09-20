#include "ResourceSetOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(const ResourceSetSpecification &spec)
        : ResourceSet(spec)
    {
        // setup texture binding names
        for (const auto &textureBinding : m_Specification.TextureBindings)
        {
            m_TextureBindingNames[textureBinding.Slot] = textureBinding.Name;
        }

        // setup uniform buffer binding names
        for (const auto &uniformBufferBindings : m_Specification.UniformResourceBindings)
        {
            m_UniformBufferBindingNames[uniformBufferBindings.Binding] = uniformBufferBindings.Name;
        }
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteTexture(Texture *texture, uint32_t binding)
    {
        OpenGLTextureBindingInfo info;
        info.Texture = texture;
        info.BindingName = m_TextureBindingNames[binding];

        m_TextureBindings[binding] = info;
    }

    void Nexus::Graphics::ResourceSetOpenGL::WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding)
    {
        OpenGLUniformBufferBindingInfo info;
        info.Buffer = uniformBuffer;
        info.BindingName = m_UniformBufferBindingNames[binding];

        m_UniformBindings[binding] = info;
    }

    const std::unordered_map<uint32_t, OpenGLTextureBindingInfo> &ResourceSetOpenGL::GetTextureBindings()
    {
        return m_TextureBindings;
    }

    const std::unordered_map<uint32_t, OpenGLUniformBufferBindingInfo> &ResourceSetOpenGL::GetUniformBufferBindings()
    {
        return m_UniformBindings;
    }
}