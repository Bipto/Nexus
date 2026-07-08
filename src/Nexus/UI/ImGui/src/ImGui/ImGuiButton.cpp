#include "ImGui/ImGuiButton.hpp"

#include <imgui.h>

namespace Nexus::UI
{
    ImGuiButton::ImGuiButton(
        std::string_view text, std::optional<Position> position, std::optional<Size> size
    )
        : m_Text(text), m_Position(position), m_Size(size)
    {
    }

    void ImGuiButton::Render()
    {
        if (m_Position)
        {
            ImGui::SetCursorPos(ImVec2(m_Position->X, m_Position->Y));
        }

        ImVec2 size = {0, 0};
        if (m_Size)
        {
            size = {static_cast<float>(m_Size->Width), static_cast<float>(m_Size->Height)};
        }

        if (ImGui::Button(m_Text.c_str(), size))
        {
            if (m_OnClick)
            {
                m_OnClick();
            }
        }
    }

    void ImGuiButton::OnClick(std::function<void()> handler)
    {
        m_OnClick = handler;
    }
} // namespace Nexus::UI