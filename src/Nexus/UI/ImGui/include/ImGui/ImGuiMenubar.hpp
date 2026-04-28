#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "UI/Menubar.hpp"

#include "ImGui/ImGuiControl.hpp"

namespace Nexus::UI
{
	class ImGuiTextMenuItem : public IMenuItem, public ImGuiControl
	{
	  public:
		ImGuiTextMenuItem(const std::string &text);
		~ImGuiTextMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

		void Render() final;

	  private:
		std::string			  m_Text	= {};
		std::function<void()> m_OnClick = {};
	};

	class ImGuiSeparatorMenuItem : public IMenuItem, public ImGuiControl
	{
	  public:
		ImGuiSeparatorMenuItem()		= default;
		~ImGuiSeparatorMenuItem() final = default;

		void OnClick(std::function<void()> handler) final;

		void Render() final;

	  private:
		std::function<void()> m_OnClick = {};
	};

	class ImGuiMenu : public IMenu, public ImGuiControl
	{
	  public:
		ImGuiMenu(const std::string &text);
		virtual ~ImGuiMenu() = default;
		IMenuItem *Append(const std::string &text) final;
		IMenu	  *AppendSubMenu(const std::string &text) final;
		IMenuItem *AppendSeparator() final;
		void	   OnMenuOpened(std::function<void()> handler) final;
		void	   OnMenuClosed(std::function<void()> handler) final;

		void Render() final;

	  private:
		std::string			  m_Text		 = {};
		std::function<void()> m_OnMenuOpened = {};
		std::function<void()> m_OnMenuClosed = {};
		bool				  m_Open		 = false;
	};

	class ImGuiMenubar : public IMenubar, public ImGuiControl
	{
	  public:
		ImGuiMenubar(bool main);
		virtual ~ImGuiMenubar() = default;
		IMenu *CreateMenu(const std::string &text) final;
		void   Render() final;

	  private:
		bool m_Main = false;
	};
}	 // namespace Nexus::UI