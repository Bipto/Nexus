#include "Nexus-Core/AssetManager.hpp"

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
#include "Nexus-Core/Runtime/Project.hpp"

namespace Nexus
{
	Ref<Graphics::Texture2D> AssetManager::GetTexture(const std::string &filepath)
	{
		return m_GraphicsDevice->CreateTexture2D(filepath.c_str(), false);
	}

	std::any AssetManager::LoadAsset(GUID id)
	{
		const Nexus::Assets::AssetRegistry &registry = m_Project->GetAssetRegistry();

		std::optional<Assets::AssetInfo> assetResult = registry.GetAssetInfo(id);
		if (!assetResult)
		{
			return {};
		}

		Assets::AssetInfo assetInfo = assetResult.value();

		std::optional<Nexus::Processors::ProcessorInfo> processorResult = m_Project->GetProcessorInfo(assetInfo.ProcessorName);
		if (processorResult)
		{
			return {};
		}

		return {};
	}

}	 // namespace Nexus
