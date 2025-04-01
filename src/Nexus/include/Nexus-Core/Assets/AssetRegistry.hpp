#pragma once

#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Assets
{
	class AssetRegistry
	{
	  public:
		AssetRegistry();
		std::optional<std::string> GetFilepath(GUID id);
		GUID					   RegisterAsset(const std::string &filepath);
		void					   RegisterAsset(const std::string &filepath, GUID id);

		const std::map<GUID, std::string> &GetStoredFilepaths() const;

	  private:
		std::map<GUID, std::string> m_Filepaths;
	};
}	 // namespace Nexus::Assets