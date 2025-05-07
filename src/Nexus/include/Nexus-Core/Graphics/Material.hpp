#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"

namespace Nexus::Graphics
{
	struct Material
	{
		Ref<Texture>   DiffuseTexture  = nullptr;
		glm::vec4	   DiffuseColour   = {};
		Ref<Texture>   NormalTexture   = nullptr;
		Ref<Texture>   SpecularTexture = nullptr;
		glm::vec4	   SpecularColour  = {};
	};
}	 // namespace Nexus::Graphics
