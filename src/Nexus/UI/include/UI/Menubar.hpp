#pragma once

#include <functional>
#include <string>

namespace Nexus::UI
{
	class IMenu
	{
	  public:
		virtual ~IMenu()															  = default;
		virtual void   Append(const std::string &text, std::function<void()> onClick) = 0;
		virtual IMenu *AppendSubMenu(const std::string &text)						  = 0;
		virtual void   AppendSeparator()											  = 0;
	};

	class IMenubar
	{
	  public:
		virtual ~IMenubar()								   = default;
		virtual IMenu *CreateMenu(const std::string &text) = 0;
	};
}	 // namespace Nexus::UI