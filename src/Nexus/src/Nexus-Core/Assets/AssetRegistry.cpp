#include "Nexus-Core/Assets/AssetRegistry.hpp"

namespace Nexus::Assets
{
	AssetRegistry::AssetRegistry()
	{
	}

	std::optional<AssetInfo> AssetRegistry::GetAssetInfo(GUID id) const
	{
		// check if the GUID has a filepath assigned
		if (m_Assets.find(id) != m_Assets.end())
		{
			return m_Assets.at(id);
		}

		// if not, return a null optional
		return {};
	}

	GUID AssetRegistry::RegisterAsset(const std::string &processor, const std::string &filepath)
	{
		// generate a new GUID
		GUID id = {};

		// check if an asset already exists with this ID
		while (m_Assets.find(id) != m_Assets.end())
		{
			// if it does, try generating a new GUID
			id = GUID();
		}

		// insert the filepath at the specified GUID
		AssetInfo info {.Filepath = filepath, .ProcessorName = processor};
		m_Assets[id] = info;
		return id;
	}

	void AssetRegistry::RegisterAsset(const std::string &processor, const std::string &filepath, GUID id)
	{
		AssetInfo info = {.Filepath = filepath, .ProcessorName = processor};
		m_Assets[id]   = info;
	}

	const std::map<GUID, AssetInfo> &Nexus::Assets::AssetRegistry::GetStoredAssets() const
	{
		return m_Assets;
	}

}	 // namespace Nexus::Assets