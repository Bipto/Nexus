#pragma once

#include <functional>
#include <string>

#include "UI/Control.hpp"

namespace Nexus::UI
{
	class IMenuItem : public IControl
	{
	  public:
		virtual ~IMenuItem()					   = default;
		virtual void OnClick(EventHandler handler) = 0;
	};

	class ITextMenuItem : public IMenuItem
	{
	  public:
		virtual ~ITextMenuItem() = default;
	};

	class ISeparatorMenuItem : public IMenuItem
	{
	  public:
		virtual ~ISeparatorMenuItem() = default;
	};

	class IMenu : public IControl
	{
	  public:
		virtual ~IMenu()										  = default;
		virtual IMenuItem *Append(const std::string &text)		  = 0;
		virtual IMenu	  *AppendSubMenu(const std::string &text) = 0;
		virtual IMenuItem *AppendSeparator()					  = 0;
		virtual void	   OnMenuOpened(EventHandler handler)	  = 0;
		virtual void	   OnMenuClosed(EventHandler handler)	  = 0;
	};

	class IMenubar : public IControl
	{
	  public:
		virtual ~IMenubar()								   = default;
		virtual IMenu *CreateMenu(const std::string &text) = 0;
	};
}	 // namespace Nexus::UI