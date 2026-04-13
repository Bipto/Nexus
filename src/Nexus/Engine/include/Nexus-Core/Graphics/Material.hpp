#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "RHI/Texture.hpp"
#include "RHI/TextureView.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
	struct Material
	{
		TextureViewHandle DiffuseTexture  = {};
		glm::vec4		  DiffuseColour	  = {};
		TextureViewHandle NormalTexture	  = {};
		TextureViewHandle SpecularTexture = {};
		glm::vec4		  SpecularColour  = {};
	};
}	 // namespace Nexus::Graphics
