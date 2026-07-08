#include "ImGui/ImGuiLayout.hpp"
#include "ImGui/ImGuiFrame.hpp"
#include "ImGui/ImGuiMenubar.hpp"
#include "ImGui/ImGuiPanel.hpp"
#include "ImGui/ImGuiStatusBar.hpp"

namespace Nexus::UI
{
    std::unique_ptr<IPanel> ImGuiLayout::CreatePanel()
    {
        return nullptr;
    }

    IFrame *ImGuiLayout::CreateFrame(const std::string &title)
    {
        m_Frame = std::make_unique<ImGuiFrame>(title);
        return m_Frame.get();
    }

    void ImGuiLayout::LogFatal(const std::string &message)
    {
    }

    void ImGuiLayout::LogError(const std::string &message)
    {
    }

    void ImGuiLayout::LogWarning(const std::string &message)
    {
    }

    void ImGuiLayout::LogMessage(const std::string &message)
    {
    }

    void ImGuiLayout::LogInfo(const std::string &message)
    {
    }

    void ImGuiLayout::ShowMessageBox(
        const std::string &title, const std::string &message
    )
    {
    }

    void ImGuiLayout::Render()
    {
        if (m_Frame)
        {
            m_Frame->Render();
        }

        for (auto &child : m_Children)
        {
            child->Render();
        }
    }
} // namespace Nexus::UI