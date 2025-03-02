#pragma once

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Mesh.hpp"
#include "Nexus-Core/Graphics/Model.hpp"
#include "Nexus-Core/nxpch.hpp"


namespace Nexus
{
	class AssimpImporter
	{
	  public:
		AssimpImporter() = default;
		Ref<Graphics::Model> Import(const std::string &filepath, Graphics::GraphicsDevice *device);
	};
}	 // namespace Nexus