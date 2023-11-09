#pragma once

#if defined(NX_PLATFORM_D3D11)

#include "Nexus/Graphics/ResourceSet.hpp"
#include "Platform/D3D11/GraphicsDeviceD3D11.hpp"

#include <unordered_map>

namespace Nexus::Graphics
{
    class ResourceSetD3D11 : public ResourceSet
    {
    public:
        ResourceSetD3D11(const ResourceSetSpecification &spec, GraphicsDeviceD3D11 *device);
        virtual void WriteTexture(Texture *texture, uint32_t binding) override;
        virtual void WriteUniformBuffer(UniformBuffer *uniformBuffer, uint32_t binding) override;

        const std::unordered_map<uint32_t, Texture *> &GetTextureBindings();
        const std::unordered_map<uint32_t, UniformBuffer *> &GetUniformBufferBindings();

    private:
        std::unordered_map<uint32_t, Texture *> m_TextureBindings;
        std::unordered_map<uint32_t, UniformBuffer *> m_UniformBufferBindings;
        GraphicsDeviceD3D11 *m_GraphicsDevice;
    };
}

#endif