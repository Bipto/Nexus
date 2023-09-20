#pragma once

#include "Nexus/Graphics/ResourceSet.hpp"
#include "Platform/DX11/GraphicsDeviceDX11.hpp"

#include <unordered_map>

namespace Nexus::Graphics
{
    class ResourceSetDX11 : public ResourceSet
    {
    public:
        ResourceSetDX11(const ResourceSetSpecification &spec, GraphicsDeviceDX11 *device);
        virtual void WriteTexture(Texture *texture, uint32_t binding) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding) override;

        const std::unordered_map<uint32_t, Texture *> &GetTextureBindings();
        const std::unordered_map<uint32_t, UniformBuffer *> &GetUniformBufferBindings();

    private:
        std::unordered_map<uint32_t, Texture *> m_TextureBindings;
        std::unordered_map<uint32_t, UniformBuffer *> m_UniformBufferBindings;
        GraphicsDeviceDX11 *m_GraphicsDevice;
    };
}