#pragma once

#include "GraphicsDevice.hpp"
#include "Texture.hpp"

#include <string>

namespace Nexus::Graphics
{
    class Font
    {
    public:
        Font(const std::string &filepath, GraphicsDevice *device);
        Nexus::Graphics::Texture *GetTexture();

    private:
        Nexus::Graphics::Texture *m_Texture = nullptr;
    };
}