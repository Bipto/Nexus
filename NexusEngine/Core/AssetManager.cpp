#include "AssetManager.h"

Nexus::Ref<Nexus::Texture> Nexus::AssetManager::GetTexture(const std::string &filepath)
{
    return m_GraphicsDevice->CreateTexture(filepath.c_str());
}