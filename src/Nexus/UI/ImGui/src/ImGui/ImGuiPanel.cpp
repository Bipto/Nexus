#include "ImGui/ImGuiPanel.hpp"

#include "ImGui/ImGuiButton.hpp"
#include "ImGui/ImGuiMenubar.hpp"

#include <format>

namespace Nexus::UI
{
	IMenubar *ImGuiPanel::CreateMenubar()
	{
		return AddChild<ImGuiMenubar, IMenubar>(false);
	}

	void ImGuiPanel::SetPosition(Position position)
	{
		m_Position = position;
	}

	void ImGuiPanel::SetSize(Size size)
	{
		m_Size = size;
	}

	IButton *ImGuiPanel::CreateButton(std::string_view text, std::optional<Position> position, std::optional<Size> size)
	{
		return AddChild<ImGuiButton, IButton>(text, position, size);
	}

	void ImGuiPanel::Render()
	{
		ImGui::PushID(this);

		if (m_Position)
		{
			ImVec2 basePos = {0, 0};

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				basePos = ImGui::GetMainViewport()->Pos;
			}

			ImGui::SetNextWindowPos(ImVec2(basePos.x + m_Position->X, basePos.y + m_Position->Y));
		}

		if (m_Size)
		{
			ImGui::SetNextWindowSize(ImVec2(m_Size->Width, m_Size->Height));
		}

		if (m_Title.empty())
		{
			ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

			if (ImGui::Begin("##",
							 &m_Open,
							 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking))
			{
				for (auto &child : m_Children) { child->Render(); }
			}
		}
		else
		{
			if (ImGui::Begin(m_Title.c_str(), &m_Open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
			{
				for (auto &child : m_Children) { child->Render(); }
			}
		}

		ImGui::End();
		ImGui::PopID();
	}
}	 // namespace Nexus::UI