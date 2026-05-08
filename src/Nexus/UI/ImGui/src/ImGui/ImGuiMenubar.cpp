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

	void ImGuiTextMenuItem::Render()
	{
		if (ImGui::MenuItem(m_Text.c_str(), nullptr, nullptr))
		{
			if (m_OnClick)
			{
				m_OnClick();
			}
		}
	}

	void ImGuiSeparatorMenuItem::OnClick(std::function<void()> handler)
	{
		m_OnClick = std::move(handler);
	}

	void ImGuiSeparatorMenuItem::Render()
	{
		ImGui::Separator();
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

	void ImGuiMenu::OnMenuOpened(std::function<void()> handler)
	{
		m_OnMenuOpened = handler;
	}

	void ImGuiMenu::OnMenuClosed(std::function<void()> handler)
	{
		m_OnMenuClosed = handler;
	}

	void ImGuiMenu::Render()
	{
		if (ImGui::BeginMenu(m_Text.c_str()))
		{
			if (!m_Open && m_OnMenuOpened)
			{
				m_OnMenuOpened();
			}

			m_Open = true;

			for (auto &child : m_Children) { child->Render(); }

			ImGui::EndMenu();
		}
		else
		{
			if (m_Open && m_OnMenuClosed)
			{
				m_OnMenuClosed();
			}

			m_Open = false;
		}
	}

	ImGuiMenubar::ImGuiMenubar(bool main) : m_Main(main)
	{
	}

	IMenu *ImGuiMenubar::CreateMenu(const std::string &text)
	{
		return AddChild<ImGuiMenu, IMenu>(text);
	}

	void ImGuiMenubar::Render()
	{
		if (m_Main)
		{
			ImGui::BeginMainMenuBar();
		}
		else
		{
			ImGui::BeginMenuBar();
		}

		for (auto &child : m_Children) { child->Render(); }

		if (m_Main)
		{
			ImGui::EndMainMenuBar();
		}
		else
		{
			ImGui::EndMenuBar();
		}
	}
}	 // namespace Nexus::UI