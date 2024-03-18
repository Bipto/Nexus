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
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name) override;
        virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;

        const std::map<std::string, Ref<TextureOpenGL>> &GetBoundTextures() const;
        const std::map<std::string, Ref<UniformBufferOpenGL>> &GetBoundUniformBuffers() const;
        const std::map<std::string, Ref<SamplerOpenGL>> &GetBoundSamplers() const;

    private:
        std::map<std::string, Ref<TextureOpenGL>> m_BoundTextures;
        std::map<std::string, Ref<UniformBufferOpenGL>> m_BoundUniformBuffers;
        std::map<std::string, Ref<SamplerOpenGL>> m_BoundSamplers;
    };
}

#endif