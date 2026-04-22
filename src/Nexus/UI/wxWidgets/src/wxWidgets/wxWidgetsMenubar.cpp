#pragma once

#include <functional>
#include <string>

#include "UI/Menubar.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"

namespace Nexus::UI
{
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

	void wxWidgetsMenu::Append(const std::string &text, std::function<void()> onClick)
	{
		int id = wxNewId();
		m_Menu->Append(id, text);
		m_OnClicks[id] = std::move(onClick);

		m_Frame->Bind(wxEVT_MENU,
					  [&](wxCommandEvent &event)
					  {
						  int id = event.GetId();
						  if (m_OnClicks.contains(id))
						  {
							  m_OnClicks[id]();
						  }
					  });
	}

	IMenu *wxWidgetsMenu::AppendSubMenu(const std::string &text)
	{
		return new wxWidgetsMenu(text, m_Menu, m_Frame);
	}

	void wxWidgetsMenu::AppendSeparator()
	{
		m_Menu->AppendSeparator();
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
		return new wxWidgetsMenu(text, m_Menubar, m_Frame);
	}
}	 // namespace Nexus::UI