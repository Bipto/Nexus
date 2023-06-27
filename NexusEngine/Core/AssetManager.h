#pragma once

#include "Core/Graphics/GraphicsDevice.h"

#include <string>

namespace Nexus
{
    class AssetManager
    {
    public:
        AssetManager(Ref<GraphicsDevice> graphicsDevice) : m_GraphicsDevice(graphicsDevice) {}

        Ref<Texture> GetTexture(const std::string &filepath);

    private:
        Ref<GraphicsDevice> m_GraphicsDevice;
    };
}