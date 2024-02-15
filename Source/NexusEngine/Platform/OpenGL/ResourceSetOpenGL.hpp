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
        virtual void WriteTexture(Texture *texture, const std::string &name) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name) override;

        const std::map<std::string, TextureOpenGL *> &GetBoundTextures() const;
        const std::map<std::string, UniformBufferOpenGL *> &GetBoundUniformBuffers() const;

    private:
        std::map<std::string, TextureOpenGL *> m_BoundTextures;
        std::map<std::string, UniformBufferOpenGL *> m_BoundUniformBuffers;
    };
}

#endif