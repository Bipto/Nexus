#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "UI/Button.hpp"
#include "UI/Control.hpp"
#include "UI/Menubar.hpp"

namespace Nexus::UI
{
	class IPanel
	{
	  public:
		virtual ~IPanel()																								  = default;
		virtual IMenubar *CreateMenubar()																				  = 0;
		virtual void	  SetPosition(Position position)																  = 0;
		virtual void	  SetSize(Size size)																			  = 0;
		virtual IButton	 *CreateButton(std::string_view text, std::optional<Position> position, std::optional<Size> size) = 0;
	};
}	 // namespace Nexus::UI