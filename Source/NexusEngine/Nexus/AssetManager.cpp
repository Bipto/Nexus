#include "AssetManager.hpp"

namespace Nexus
{
    Ref<Graphics::Texture> AssetManager::GetTexture(const std::string &filepath)
    {
        return m_GraphicsDevice->CreateTexture(filepath.c_str());
    }
}
