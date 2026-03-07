#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "RHI/Texture.hpp"
#include "RHI/TextureView.hpp"
#include "RHI/Types.hpp"

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
