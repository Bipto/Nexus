#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/ResourceSet.hpp"

#include "Platform/OpenGL/TextureOpenGL.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"

#include <unordered_map>
#include <variant>

namespace Nexus::Graphics
{
    struct OpenGLResourceInfo
    {
        ResourceType Type;
        std::variant<TextureOpenGL *, UniformBufferOpenGL *> Resource;
    };

    class ResourceSetOpenGL : public ResourceSet
    {
    public:
        ResourceSetOpenGL(const ResourceSetSpecification &spec);
        virtual void WriteTexture(Texture *texture, uint32_t set, uint32_t binding) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t set, uint32_t binding) override;

        const std::map<uint32_t, OpenGLResourceInfo> &GetResources() const;

    private:
        std::map<uint32_t, OpenGLResourceInfo> m_Resources;
    };
}

#endif