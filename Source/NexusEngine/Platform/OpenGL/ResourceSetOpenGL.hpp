#pragma once

#include "Nexus/Graphics/ResourceSet.hpp"
#include <unordered_map>

namespace Nexus::Graphics
{
    struct OpenGLTextureBindingInfo
    {
        Ref<Texture> Texture;
        std::string BindingName;
    };

    struct OpenGLUniformBufferBindingInfo
    {
        Ref<UniformBuffer> Buffer;
        std::string BindingName;
    };

    class ResourceSetOpenGL : public ResourceSet
    {
    public:
        ResourceSetOpenGL(const ResourceSetSpecification &spec);
        virtual void WriteTexture(Ref<Texture> texture, uint32_t binding) override;
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding) override;

        const std::unordered_map<uint32_t, OpenGLTextureBindingInfo> &GetTextureBindings();
        const std::unordered_map<uint32_t, OpenGLUniformBufferBindingInfo> &GetUniformBufferBindings();

    private:
        std::unordered_map<uint32_t, OpenGLTextureBindingInfo> m_TextureBindings;
        std::unordered_map<uint32_t, OpenGLUniformBufferBindingInfo> m_UniformBindings;

        std::unordered_map<uint32_t, std::string> m_TextureBindingNames;
        std::unordered_map<uint32_t, std::string> m_UniformBufferBindingNames;
    };
}