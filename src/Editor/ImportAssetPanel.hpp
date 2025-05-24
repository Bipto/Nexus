#pragma once
#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Panel.hpp"

class ImportAssetPanel : public Panel
{
  public:
	ImportAssetPanel() : Panel("Import Asset")
	{
	}

	inline std::vector<std::string> GetProcessorsForFiles(const std::vector<std::string> &files)
	{
		const auto				&availableProcessors = m_Project->GetCachedAvailableAssetProcessors();
		std::vector<std::string> processors			 = {};

		for (const auto &filepath : files)
		{
			std::filesystem::path fullPath		 = filepath;
			std::string			  assetExtension = fullPath.extension().string();
			bool				  found			 = false;

			for (const auto &[processorName, processorInfo] : availableProcessors)
			{
				for (const auto &extension : processorInfo.FileExtensions)
				{
					if (assetExtension == extension)
					{
						processors.push_back(processorName);
						found = true;
					}
				}
			}

			if (!found)
			{
				processors.push_back(std::string {});
			}
		}

		return processors;
	}

	void Render() final
	{
		ImGui::Begin(m_Name.c_str(), &m_Open);

		if (m_Project)
		{
			std::vector<std::string> processorNames = GetProcessorsForFiles(m_Filepaths);
			std::optional<size_t>	 pathToRemove	= {};

			for (size_t i = 0; i < processorNames.size(); i++)
			{
				std::string filepath  = m_Filepaths[i];
				std::string processorName = processorNames[i];

				if (ImGui::CollapsingHeader(filepath.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (processorName.empty())
					{
						ImGui::Text("No processor was found for this file type");
					}
					else
					{
						ImGui::Text(processorName.c_str());
						ImGui::PushID(i);
						if (ImGui::Button("Import"))
						{
							std::optional<Nexus::Processors::ProcessorInfo> info = m_Project->GetProcessorInfo(processorName);
							if (info)
							{
								std::unique_ptr<Nexus::Processors::IProcessor> processor =
									std::unique_ptr<Nexus::Processors::IProcessor>(info.value().CreationFunction());
								processor->Process(filepath, Nexus::GetApplication()->GetGraphicsDevice(), m_Project.get());
								pathToRemove = i;
							}
						}
						ImGui::PopID();
					}
				}
			}

			if (pathToRemove)
			{
				m_Filepaths.erase(m_Filepaths.begin() + pathToRemove.value());
			}
		}

		ImGui::End();
	}

	void SetFilepaths(const std::vector<std::string> filepaths)
	{
		m_Filepaths = filepaths;
	}

  private:
	std::vector<std::string> m_Filepaths = {};
};