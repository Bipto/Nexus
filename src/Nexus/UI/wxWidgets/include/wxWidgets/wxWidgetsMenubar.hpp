#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Menubar.hpp"

namespace Nexus::UI
{
	class wxWidgetsMenu : public IMenu
	{
	  public:
		wxWidgetsMenu(const std::string &text, wxMenuBar *menubar, wxFrame *frame);
		wxWidgetsMenu(const std::string &text, wxMenu *parent, wxFrame *frame);
		virtual ~wxWidgetsMenu() = default;
		void   Append(const std::string &text, std::function<void()> onClick) final;
		IMenu *AppendSubMenu(const std::string &text) final;
		void   AppendSeparator() final;

		wxMenu *GetMenu();

	  private:
		wxMenuBar													*m_Menubar	  = nullptr;
		wxMenu														*m_ParentMenu = nullptr;
		wxMenu														*m_Menu		  = nullptr;
		wxFrame														*m_Frame	  = nullptr;
		inline static std::unordered_map<int, std::function<void()>> m_OnClicks	  = {};
	};

	class wxWidgetsMenubar : public IMenubar
	{
	  public:
		wxWidgetsMenubar(wxFrame *frame);
		virtual ~wxWidgetsMenubar() = default;
		IMenu *CreateMenu(const std::string &text) final;

	  private:
		wxFrame	  *m_Frame	 = nullptr;
		wxMenuBar *m_Menubar = nullptr;
	};
}	 // namespace Nexus::UI