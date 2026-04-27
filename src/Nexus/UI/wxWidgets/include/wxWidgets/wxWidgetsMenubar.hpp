#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Menubar.hpp"

namespace Nexus::UI
{
	class wxWidgetsTextMenuItem : public ITextMenuItem
	{
	  public:
		wxWidgetsTextMenuItem(const std::string &text, wxMenu *menu, wxFrame *frame);
		~wxWidgetsTextMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

	  private:
		wxMenuItem			 *m_MenuItem = nullptr;
		std::function<void()> m_OnClick	 = {};
	};

	class wxWidgetsSeparatorMenuItem : public ISeparatorMenuItem
	{
	  public:
		wxWidgetsSeparatorMenuItem(wxMenu *menu, wxFrame *frame);
		~wxWidgetsSeparatorMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

	  private:
		wxMenuItem			 *m_MenuItem = nullptr;
		std::function<void()> m_OnClick	 = {};
	};

	class wxWidgetsMenu : public IMenu
	{
	  public:
		wxWidgetsMenu(const std::string &text, wxMenuBar *menubar, wxFrame *frame);
		wxWidgetsMenu(const std::string &text, wxMenu *parent, wxFrame *frame);
		virtual ~wxWidgetsMenu() = default;
		IMenuItem *Append(const std::string &text) final;
		IMenu	  *AppendSubMenu(const std::string &text) final;
		IMenuItem *AppendSeparator() final;
		void	   OnMenuOpened(EventHandler handler) final;
		void	   OnMenuClosed(EventHandler handler) final;

		wxMenu *GetMenu();

	  private:
		wxMenuBar *m_Menubar	= nullptr;
		wxMenu	  *m_ParentMenu = nullptr;
		wxMenu	  *m_Menu		= nullptr;
		wxFrame	  *m_Frame		= nullptr;

		std::function<void()> m_OnMenuOpened = {};
		std::function<void()> m_OnMenuClosed = {};
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