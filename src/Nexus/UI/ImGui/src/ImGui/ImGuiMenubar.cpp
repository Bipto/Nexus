#pragma once

#include <functional>
#include <string>

#include "UI/Menubar.hpp"

#include "ImGui/ImGuiMenubar.hpp"

namespace Nexus::UI
{
	ImGuiTextMenuItem::ImGuiTextMenuItem(const std::string &text) : m_Text(text)
	{
	}

	void ImGuiTextMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClick = std::move(handler);
	}

	void ImGuiSeparatorMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClick = std::move(handler);
	}

	ImGuiMenu::ImGuiMenu(const std::string &text) : m_Text(text)
	{
	}

	IMenuItem *ImGuiMenu::Append(const std::string &text)
	{
		return AddChild<ImGuiTextMenuItem, IMenuItem>(text);
	}

	IMenu *ImGuiMenu::AppendSubMenu(const std::string &text)
	{
		return AddChild<ImGuiMenu, IMenu>(text);
	}

	IMenuItem *ImGuiMenu::AppendSeparator()
	{
		return AddChild<ImGuiSeparatorMenuItem, IMenuItem>();
	}

	void ImGuiMenu::OnMenuOpened(EventHandler handler)
	{
	}

	void ImGuiMenu::OnMenuClosed(EventHandler handler)
	{
	}

	IMenu *ImGuiMenubar::CreateMenu(const std::string &text)
	{
		return AddChild<ImGuiMenu, IMenu>(text);
	}
}	 // namespace Nexus::UI