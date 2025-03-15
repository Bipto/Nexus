#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Assets
{
	class AssetRegistry;

	class IImporter
	{
	  public:
		IImporter()																	   = default;
		virtual ~IImporter()														   = default;
		virtual bool Import(const std::string &filepath, AssetRegistry *assetRegistry) = 0;
	};
}	 // namespace Nexus::Assets