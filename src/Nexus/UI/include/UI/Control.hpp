#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace Nexus::UI
{
	struct Position
	{
		uint32_t X = 0;
		uint32_t Y = 0;
	};

	struct Size
	{
		uint32_t Width	= 0;
		uint32_t Height = 0;
	};

	class IControl
	{
	  public:
		virtual ~IControl() = default;
	};
}	 // namespace Nexus::UI