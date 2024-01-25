#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/ResourceSet.hpp"

#include "Platform/OpenGL/TextureOpenGL.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"

#include <unordered_map>

namespace Nexus::Graphics
{
    class ResourceSetOpenGL : public ResourceSet
    {
    public:
        ResourceSetOpenGL(const ResourceSetSpecification &spec);
        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) override;

        const std::map<uint32_t, TextureOpenGL *> &GetTextureBindings() const;
        const std::map<uint32_t, UniformBufferOpenGL *> &GetUniformBufferBindings() const;

    private:
        std::map<uint32_t, TextureOpenGL *> m_TextureBindings;
        std::map<uint32_t, UniformBufferOpenGL *> m_UniformBufferBindings;
    };
}

#endif