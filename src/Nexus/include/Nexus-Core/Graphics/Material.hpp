#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"

namespace Nexus::Graphics
{
	struct Material
	{
		Ref<ITextureView> DiffuseTexture  = nullptr;
		glm::vec4		  DiffuseColour	  = {};
		Ref<ITextureView> NormalTexture	  = nullptr;
		Ref<ITextureView> SpecularTexture = nullptr;
		glm::vec4		  SpecularColour  = {};
	};
}	 // namespace Nexus::Graphics
