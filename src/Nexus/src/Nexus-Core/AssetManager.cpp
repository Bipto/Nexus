#include "Nexus-Core/AssetManager.hpp"

namespace Nexus
{
	Ref<Graphics::Texture2D> AssetManager::GetTexture(const std::string &filepath)
	{
		return m_GraphicsDevice->CreateTexture2D(filepath.c_str(), false);
	}
}	 // namespace Nexus
