#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/Runtime/Entity.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Panel.hpp"

namespace Editor
{
	class Layout
	{
	  public:
		explicit Layout(Nexus::Application *app);
		void		Render(Nexus::TimeSpan time);
		static void LoadProject(const std::string &path);

		static void SaveLayout(const std::string &path);
		static bool LoadLayout(const std::string &path);

	  public:
		static Nexus::Entity *s_SelectedEntity;

	  private:
		void		OpenProject();
		void		RenderViewport();
		static void ApplyDarkTheme();
		void		RenderMainMenubar();

	  private:
		std::unique_ptr<Nexus::ImGuiUtils::ImGuiGraphicsRenderer> m_ImGuiRenderer = nullptr;
		Nexus::Application										 *m_Application	  = nullptr;

		std::map<std::string, std::unique_ptr<Panel>> m_Panels;
		std::map<std::string, std::unique_ptr<Panel>> m_Dialogs;
	};
}	 // namespace Editor