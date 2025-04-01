#include "Nexus-Core/Assets/AssetRegistry.hpp"

namespace Nexus::Assets
{
	AssetRegistry::AssetRegistry()
	{
	}

	std::optional<std::string> AssetRegistry::GetFilepath(GUID id)
	{
		// check if the GUID has a filepath assigned
		if (m_Filepaths.find(id) != m_Filepaths.end())
		{
			return m_Filepaths[id];
		}

		// if not, return a null optional
		return {};
	}

	GUID AssetRegistry::RegisterAsset(const std::string &filepath)
	{
		// generate a new GUID
		GUID id = {};

		// check if the GUID already exists within the map
		while (m_Filepaths.find(id) != m_Filepaths.end())
		{
			// if it does, try generating a new GUID
			id = GUID();
		}

		// insert the filepath at the specified GUID
		m_Filepaths[id] = filepath;
		return id;
	}

	void AssetRegistry::RegisterAsset(const std::string &filepath, GUID id)
	{
		m_Filepaths[id] = filepath;
	}

	const std::map<GUID, std::string> &Nexus::Assets::AssetRegistry::GetStoredFilepaths() const
	{
		return m_Filepaths;
	}

}	 // namespace Nexus::Assets