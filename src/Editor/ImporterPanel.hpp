#pragma once

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
#include "Panel.hpp"

class ImporterPanel : public Panel
{
  public:
	ImporterPanel() : Panel("Importers")
	{
	}

	void Render() final
	{
		ImGui::Begin(m_Name.c_str(), &m_Open);

		if (m_Project)
		{
			const auto &availableProcessors = Nexus::Processors::ProcessorRegistry::GetRegistry();
			for (const auto &[name, info] : availableProcessors)
			{
				if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					for (const auto &extension : info.FileExtensions) { ImGui::Text(extension.c_str()); }
				}
			}
		}

		ImGui::End();
	}
};