#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/nxpch.hpp"
#include "Nexus-Core/Graphics/ResourceSet.hpp"

#include "Platform/OpenGL/TextureOpenGL.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"
#include "Platform/OpenGL/SamplerOpenGL.hpp"

namespace Nexus::Graphics
{
    class ResourceSetOpenGL : public ResourceSet
    {
    public:
        ResourceSetOpenGL(const ResourceSetSpecification &spec);
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, const std::string &name) override;
        virtual void WriteCombinedImageSampler(Ref<Texture> texture, Ref<Sampler> sampler, const std::string &name) override;

        const std::map<std::string, WeakRef<TextureOpenGL>> &GetBoundTextures() const;
        const std::map<std::string, WeakRef<UniformBufferOpenGL>> &GetBoundUniformBuffers() const;
        const std::map<std::string, WeakRef<SamplerOpenGL>> &GetBoundSamplers() const;

    private:
        std::map<std::string, WeakRef<TextureOpenGL>> m_BoundTextures;
        std::map<std::string, WeakRef<UniformBufferOpenGL>> m_BoundUniformBuffers;
        std::map<std::string, WeakRef<SamplerOpenGL>> m_BoundSamplers;
    };
}

#endif