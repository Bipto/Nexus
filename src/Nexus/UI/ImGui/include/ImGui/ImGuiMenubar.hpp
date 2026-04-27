#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "UI/Menubar.hpp"

namespace Nexus::UI
{
	class ImGuiTextMenuItem : public IMenuItem
	{
	  public:
		ImGuiTextMenuItem(const std::string &text);
		~ImGuiTextMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

	  private:
		std::string			  m_Text	= {};
		std::function<void()> m_OnClick = {};
	};

	class ImGuiSeparatorMenuItem : public IMenuItem
	{
	  public:
		ImGuiSeparatorMenuItem()		= default;
		~ImGuiSeparatorMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

	  private:
		std::function<void()> m_OnClick = {};
	};

	class ImGuiMenu : public IMenu
	{
	  public:
		ImGuiMenu(const std::string &text);
		virtual ~ImGuiMenu() = default;
		IMenuItem *Append(const std::string &text) final;
		IMenu	  *AppendSubMenu(const std::string &text) final;
		IMenuItem *AppendSeparator() final;
		void	   OnMenuOpened(EventHandler handler) final;
		void	   OnMenuClosed(EventHandler handler) final;

	  private:
		std::string m_Text = {};
	};

	class ImGuiMenubar : public IMenubar
	{
	  public:
		ImGuiMenubar()			= default;
		virtual ~ImGuiMenubar() = default;
		IMenu *CreateMenu(const std::string &text) final;
	};
}	 // namespace Nexus::UI