#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"

namespace Nexus::Graphics
{
	struct Material
	{
		Ref<Texture2D> DiffuseTexture  = nullptr;
		glm::vec4	   DiffuseColour   = {};
		Ref<Texture2D> NormalTexture   = nullptr;
		Ref<Texture2D> SpecularTexture = nullptr;
		glm::vec4	   SpecularColour  = {};
	};
}	 // namespace Nexus::Graphics
