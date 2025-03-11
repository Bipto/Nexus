#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::UI
{
	struct Rounding
	{
		float TopLeft	  = 0.0f;
		float TopRight	  = 0.0f;
		float BottomLeft  = 0.0f;
		float BottomRight = 0.0f;
	};

	struct Style
	{
		glm::vec4 BackgroundColour = {1.0f, 1.0f, 1.0f, 1.0f};
		glm::vec4 ForegroundColour = {0.0f, 0.0f, 0.0f, 1.0f};
		Rounding  CornerRounding   = {};
	};
}	 // namespace Nexus::UI