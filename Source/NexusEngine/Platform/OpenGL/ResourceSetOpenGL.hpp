#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/ResourceSet.hpp"

#include "Platform/OpenGL/TextureOpenGL.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"
#include "Platform/OpenGL/SamplerOpenGL.hpp"

#include <unordered_map>
#include <variant>

namespace Nexus::Graphics
{
    class ResourceSetOpenGL : public ResourceSet
    {
    public:
        ResourceSetOpenGL(const ResourceSetSpecification &spec);
        virtual void PerformResourceUpdate() override;
        virtual void WriteTexture(Texture *texture, const std::string &name) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, const std::string &name) override;
        virtual void WriteSampler(Sampler *sampler, const std::string &name) override;
        virtual void WriteCombinedImageSampler(Texture *texture, Sampler *sampler, const std::string &name) override;

        const std::map<std::string, TextureOpenGL *> &GetBoundTextures() const;
        const std::map<std::string, UniformBufferOpenGL *> &GetBoundUniformBuffers() const;
        const std::map<std::string, SamplerOpenGL *> &GetBoundSamplers() const;

    private:
        std::map<std::string, TextureOpenGL *> m_BoundTextures;
        std::map<std::string, UniformBufferOpenGL *> m_BoundUniformBuffers;
        std::map<std::string, SamplerOpenGL *> m_BoundSamplers;
    };
}

#endif