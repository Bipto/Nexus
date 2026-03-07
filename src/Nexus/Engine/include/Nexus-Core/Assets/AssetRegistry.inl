#pragma once

namespace Nexus::Assets
{
	template<typename T>
	std::map<GUID, AssetInfo> AssetRegistry::GetAllAssetsOfType() const
	{
		std::map<GUID, AssetInfo> result;
		return result;
	}
}	 // namespace Nexus::Assets