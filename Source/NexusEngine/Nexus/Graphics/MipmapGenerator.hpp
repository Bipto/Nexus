#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "Nexus/Graphics/FullscreenQuad.hpp"

namespace Nexus::Graphics
{
    class MipmapGenerator
    {
    public:
        MipmapGenerator(GraphicsDevice *device);
        void GenerateMip(Ref<Texture> texture);
        std::vector<std::vector<std::byte>> GenerateMips(Ref<Texture> texture, uint32_t mipCount);

        static uint32_t GetMaximumNumberOfMips(uint32_t width, uint32_t height);

    private:
        GraphicsDevice *m_Device = nullptr;
        Ref<CommandList> m_CommandList = nullptr;
        FullscreenQuad m_Quad;
    };
}