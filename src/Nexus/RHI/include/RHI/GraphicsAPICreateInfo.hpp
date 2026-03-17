#pragma once

#include "RHI/APIVersion.hpp"

namespace Nexus::Graphics
{
	struct GraphicsAPICreateInfo
	{
		GraphicsAPI API	  = GraphicsAPI::OpenGL;
		bool		Debug = false;
	};

}	 // namespace Nexus::Graphics
