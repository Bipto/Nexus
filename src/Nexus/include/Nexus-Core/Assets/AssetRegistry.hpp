#pragma once

#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Assets
{
	struct AssetInfo
	{
		std::string Filepath	  = {};
		std::string ProcessorName = {};
	};

	class AssetRegistry
	{
	  public:
		AssetRegistry();
		std::optional<AssetInfo> GetAssetInfo(GUID id) const;
		GUID					 RegisterAsset(const std::string &processor, const std::string &filepath);
		void					 RegisterAsset(const std::string &processor, const std::string &filepath, GUID id);

		const std::map<GUID, AssetInfo> &GetStoredAssets() const;

	  private:
		std::map<GUID, AssetInfo> m_Assets;
	};
}	 // namespace Nexus::Assets