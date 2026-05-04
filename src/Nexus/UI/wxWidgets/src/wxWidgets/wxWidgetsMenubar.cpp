#include <functional>
#include <string>

#include "UI/Menubar.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"

namespace Nexus::UI
{
	wxWidgetsTextMenuItem::wxWidgetsTextMenuItem(const std::string &text, wxMenu *menu, wxFrame *frame)
		: m_Frame(frame),
		  m_MenuItem(menu->Append(wxNewId(), text))
	{
		m_OnClickEventBinder = std::make_unique<EventBinder>(m_Frame, wxEVT_MENU, m_MenuItem->GetId());
	}

	void wxWidgetsTextMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClickEventBinder->SetCallback(handler);
	}

	wxWidgetsSeparatorMenuItem::wxWidgetsSeparatorMenuItem(wxMenu *menu, wxFrame *frame) : m_Frame(frame), m_MenuItem(menu->AppendSeparator())
	{
		m_OnClickEventBinder = std::make_unique<EventBinder>(m_Frame, wxEVT_MENU, m_MenuItem->GetId());
	}

	void wxWidgetsSeparatorMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClickEventBinder->SetCallback(handler);
	}

	wxWidgetsMenu::wxWidgetsMenu(const std::string &text, wxMenuBar *menubar, wxFrame *frame)
		: m_Menubar(menubar),
		  m_Menu(new wxMenu()),
		  m_Frame(frame)
	{
		m_Menubar->Append(m_Menu, text);
	}

	wxWidgetsMenu::wxWidgetsMenu(const std::string &text, wxMenu *parent, wxFrame *frame) : m_ParentMenu(parent), m_Menu(new wxMenu()), m_Frame(frame)
	{
		m_ParentMenu->AppendSubMenu(m_Menu, text);
	}

	IMenuItem *wxWidgetsMenu::Append(const std::string &text)
	{
		m_MenuItems.push_back(std::make_unique<wxWidgetsTextMenuItem>(text, m_Menu, m_Frame));
		return dynamic_cast<IMenuItem *>(m_MenuItems.back().get());
	}

	IMenu *wxWidgetsMenu::AppendSubMenu(const std::string &text)
	{
		m_MenuItems.push_back(std::make_unique<wxWidgetsMenu>(text, m_Menu, m_Frame));
		return dynamic_cast<IMenu *>(m_MenuItems.back().get());
	}

	IMenuItem *wxWidgetsMenu::AppendSeparator()
	{
		m_MenuItems.push_back(std::make_unique<wxWidgetsSeparatorMenuItem>(m_Menu, m_Frame));
		return dynamic_cast<IMenuItem *>(m_MenuItems.back().get());
	}

	void wxWidgetsMenu::OnMenuOpened(std::function<void()> handler)
	{
		m_OnMenuOpened = std::move(handler);
	}

	void wxWidgetsMenu::OnMenuClosed(std::function<void()> handler)
	{
		m_OnMenuClosed = std::move(handler);
	}

	wxMenu *wxWidgetsMenu::GetMenu()
	{
		return m_Menu;
	}

	void wxWidgetsMenu::InvokeOnMenuOpened()
	{
		if (m_OnMenuOpened)
		{
			m_OnMenuOpened();
		}
	}

	void wxWidgetsMenu::InvokeOnMenuClosed()
	{
		if (m_OnMenuClosed)
		{
			m_OnMenuClosed();
		}
	}

	wxWidgetsMenubar::wxWidgetsMenubar(wxFrame *frame) : m_Frame(frame), m_Menubar(new wxMenuBar())
	{
		m_Frame->SetMenuBar(m_Menubar);

		/*m_OnMenuOpenBinding = std::make_unique<WxEventBinding<wxMenuEvent>>(m_Frame,
																			wxEVT_MENU_OPEN,
																			m_Menubar->GetId(),
																			[this](wxMenuEvent &event)
																			{
																				for (auto &menu : m_Menus)
																				{
																					if (event.GetMenu() == menu->GetMenu())
																					{
																						menu->InvokeOnMenuOpened();
																					}
																				}
																			});

		m_OnMenuCloseBinding = std::make_unique<WxEventBinding<wxMenuEvent>>(m_Frame,
																			 wxEVT_MENU_CLOSE,
																			 m_Menubar->GetId(),
																			 [this](wxMenuEvent &event)
																			 {
																				 for (auto &menu : m_Menus)
																				 {
																					 if (event.GetMenu() == menu->GetMenu())
																					 {
																						 menu->InvokeOnMenuClosed();
																					 }
																				 }
																			 });*/
	}

	IMenu *wxWidgetsMenubar ::CreateMenu(const std::string &text)
	{
		m_Menus.push_back(std::make_unique<wxWidgetsMenu>(text, m_Menubar, m_Frame));
		return m_Menus.back().get();
	}
}	 // namespace Nexus::UI