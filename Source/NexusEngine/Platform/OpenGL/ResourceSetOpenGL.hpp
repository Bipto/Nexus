#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/ResourceSet.hpp"

#include "Platform/OpenGL/TextureOpenGL.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"

#include <unordered_map>
#include <variant>

namespace Nexus::Graphics
{
    class ResourceSetOpenGL : public ResourceSet
    {
    public:
        ResourceSetOpenGL(const ResourceSetSpecification &spec);
        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) override;

        const std::map<uint32_t, TextureOpenGL *> &GetBoundTextures() const;
        const std::map<uint32_t, UniformBufferOpenGL *> &GetBoundUniformBuffers() const;

    private:
        std::map<uint32_t, TextureOpenGL *> m_BoundTextures;
        std::map<uint32_t, UniformBufferOpenGL *> m_BoundUniformBuffers;
    };
}

#endif