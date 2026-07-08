#pragma once

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "RHI/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
    class MipmapGenerator
    {
      public:
        MipmapGenerator() = default;
        explicit MipmapGenerator(IGraphicsDevice *device, Graphics::CommandQueueHandle commandQueue);
        std::vector<char> GenerateMip(
            TextureHandle texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom, uint32_t arrayLayer
        );

        static uint32_t GetMaximumNumberOfMips(uint32_t width, uint32_t height);

      private:
        IGraphicsDevice *m_Device = nullptr;
        CommandListHandle m_CommandList = {};
        FullscreenQuad m_Quad{};

        PipelineHandle m_Pipeline = {};
        ResourceSetHandle m_ResourceSet = {};

        Graphics::CommandQueueHandle m_CommandQueue = {};
    };
} // namespace Nexus::Graphics