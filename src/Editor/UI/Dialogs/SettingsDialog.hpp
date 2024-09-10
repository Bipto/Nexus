#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "UI/Panel.hpp"

namespace Editor
{
	class SettingsDialog : public Panel
	{
	  public:
		explicit SettingsDialog(std::map<std::string, std::unique_ptr<Panel>> *panels);
		virtual void OnLoad() override;
		virtual void OnRender() override;

	  private:
		std::map<std::string, std::unique_ptr<Panel>> *m_Panels;
	};
}	 // namespace Editor