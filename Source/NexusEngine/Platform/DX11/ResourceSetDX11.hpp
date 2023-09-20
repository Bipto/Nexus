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
        virtual void WriteTexture(Ref<Texture> texture, uint32_t binding) override;
        virtual void WriteUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding) override;

        const std::unordered_map<uint32_t, Ref<Texture>> &GetTextureBindings();
        const std::unordered_map<uint32_t, Ref<UniformBuffer>> &GetUniformBufferBindings();

    private:
        std::unordered_map<uint32_t, Ref<Texture>> m_TextureBindings;
        std::unordered_map<uint32_t, Ref<UniformBuffer>> m_UniformBufferBindings;
        GraphicsDeviceDX11 *m_GraphicsDevice;
    };
}