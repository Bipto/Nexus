#pragma once

#include "Nexus-Core/Assets/AssetRegistry.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Processors
{
	class NX_API IProcessor
	{
	  public:
		IProcessor()									  = default;
		virtual ~IProcessor()							  = default;
		virtual GUID Process(const std::string &filepath, Graphics::GraphicsDevice *device, Assets::AssetRegistry *registry) = 0;
	};
}	 // namespace Nexus::Processors