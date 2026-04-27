#include <functional>
#include <string>

#include "UI/Menubar.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"

namespace Nexus::UI
{
	wxWidgetsTextMenuItem::wxWidgetsTextMenuItem(const std::string &text, wxMenu *menu, wxFrame *frame) : m_MenuItem(menu->Append(wxNewId(), text))
	{
		frame->Bind(
			wxEVT_MENU,
			[this](wxCommandEvent &event)
			{
				if (m_OnClick)
				{
					m_OnClick();
				}
			},
			m_MenuItem->GetId());
	}

	void wxWidgetsTextMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClick = handler;
	}

	wxWidgetsSeparatorMenuItem::wxWidgetsSeparatorMenuItem(wxMenu *menu, wxFrame *frame) : m_MenuItem(menu->AppendSeparator())
	{
		frame->Bind(
			wxEVT_MENU,
			[this](wxCommandEvent &event)
			{
				if (m_OnClick)
				{
					m_OnClick();
				}
			},
			m_MenuItem->GetId());
	}

	void wxWidgetsSeparatorMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClick = handler;
	}

	wxWidgetsMenu::wxWidgetsMenu(const std::string &text, wxMenuBar *menubar, wxFrame *frame)
		: m_Menubar(menubar),
		  m_Menu(new wxMenu()),
		  m_Frame(frame)
	{
		m_Menubar->Append(m_Menu, text);

		frame->Bind(
			wxEVT_MENU_OPEN,
			[this](wxMenuEvent &event)
			{
				if (m_OnMenuOpened)
					m_OnMenuOpened();
			},
			m_Menubar->GetId());

		frame->Bind(
			wxEVT_MENU_CLOSE,
			[this](wxMenuEvent &event)
			{
				if (m_OnMenuClosed)
					m_OnMenuClosed();
			},
			m_Menubar->GetId());
	}

	wxWidgetsMenu::wxWidgetsMenu(const std::string &text, wxMenu *parent, wxFrame *frame) : m_ParentMenu(parent), m_Menu(new wxMenu()), m_Frame(frame)
	{
		m_ParentMenu->AppendSubMenu(m_Menu, text);
	}

	IMenuItem *wxWidgetsMenu::Append(const std::string &text)
	{
		return AddChild<wxWidgetsTextMenuItem, IMenuItem>(text, m_Menu, m_Frame);
	}

	IMenu *wxWidgetsMenu::AppendSubMenu(const std::string &text)
	{
		return AddChild<wxWidgetsMenu, IMenu>(text, m_Menu, m_Frame);
	}

	IMenuItem *wxWidgetsMenu::AppendSeparator()
	{
		return AddChild<wxWidgetsSeparatorMenuItem, IMenuItem>(m_Menu, m_Frame);
	}

	void wxWidgetsMenu::OnMenuOpened(EventHandler handler)
	{
		m_OnMenuOpened = handler;
	}

	void wxWidgetsMenu::OnMenuClosed(EventHandler handler)
	{
		m_OnMenuClosed = handler;
	}

	wxMenu *wxWidgetsMenu::GetMenu()
	{
		return m_Menu;
	}

	wxWidgetsMenubar::wxWidgetsMenubar(wxFrame *frame) : m_Frame(frame), m_Menubar(new wxMenuBar())
	{
		frame->SetMenuBar(m_Menubar);
	}

	IMenu *wxWidgetsMenubar ::CreateMenu(const std::string &text)
	{
		return AddChild<wxWidgetsMenu, IMenu>(text, m_Menubar, m_Frame);
	}
}	 // namespace Nexus::UI