#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Menubar.hpp"

#include "wxWidgets/EventBinder.hpp"

namespace Nexus::UI
{
	class wxWidgetsTextMenuItem final : public ITextMenuItem
	{
	  public:
		wxWidgetsTextMenuItem(const std::string &text, wxMenu *menu, wxFrame *frame);
		~wxWidgetsTextMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

	  private:
		wxFrame	   *m_Frame = nullptr;
		wxMenuItem *m_MenuItem;

		std::unique_ptr<EventBinder> m_OnClickEventBinder = nullptr;
	};

	class wxWidgetsSeparatorMenuItem final : public ISeparatorMenuItem
	{
	  public:
		wxWidgetsSeparatorMenuItem(wxMenu *menu, wxFrame *frame);
		~wxWidgetsSeparatorMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

	  private:
		wxFrame	   *m_Frame	   = nullptr;
		wxMenuItem *m_MenuItem = nullptr;

		std::unique_ptr<EventBinder> m_OnClickEventBinder = nullptr;
	};

	class wxWidgetsMenu final : public IMenu
	{
	  public:
		wxWidgetsMenu(const std::string &text, wxMenuBar *menubar, wxFrame *frame);
		wxWidgetsMenu(const std::string &text, wxMenu *parent, wxFrame *frame);
		virtual ~wxWidgetsMenu() = default;
		IMenuItem *Append(const std::string &text) final;
		IMenu	  *AppendSubMenu(const std::string &text) final;
		IMenuItem *AppendSeparator() final;
		void	   OnMenuOpened(std::function<void()> handler) final;
		void	   OnMenuClosed(std::function<void()> handler) final;

		wxMenu *GetMenu();

		void InvokeOnMenuOpened();
		void InvokeOnMenuClosed();

	  private:
		wxMenuBar *m_Menubar	= nullptr;
		wxMenu	  *m_ParentMenu = nullptr;
		wxMenu	  *m_Menu		= nullptr;
		wxFrame	  *m_Frame		= nullptr;

		std::function<void()> m_OnMenuOpened;
		std::function<void()> m_OnMenuClosed;

		std::vector<std::unique_ptr<IMenuBase>> m_MenuItems = {};
	};

	class wxWidgetsMenubar final : public IMenubar
	{
	  public:
		wxWidgetsMenubar(wxFrame *frame);
		virtual ~wxWidgetsMenubar() = default;
		IMenu *CreateMenu(const std::string &text) final;

	  private:
		wxFrame	  *m_Frame	 = nullptr;
		wxMenuBar *m_Menubar = nullptr;

		std::vector<std::unique_ptr<wxWidgetsMenu>> m_Menus = {};
	};
}	 // namespace Nexus::UI