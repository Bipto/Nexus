#pragma once

#include <vector>

#include <memory>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "UI/Control.hpp"
#include "UI/Layout.hpp"

#include "ImGui/ImGuiControl.hpp"
#include "ImGui/ImGuiFrame.hpp"

namespace Nexus::UI
{
    class ImGuiLayout final : public ILayout, public ImGuiControl
    {
      public:
        ImGuiLayout() = default;
        ~ImGuiLayout() final = default;
        std::unique_ptr<IPanel> CreatePanel() final;
        IFrame *CreateFrame(const std::string &title) final;

        void LogFatal(const std::string &message) final;
        void LogError(const std::string &message) final;
        void LogWarning(const std::string &message) final;
        void LogMessage(const std::string &message) final;
        void LogInfo(const std::string &message) final;
        void ShowMessageBox(
            const std::string &title, const std::string &message
        ) final;

        void Render() final;

      private:
        std::unique_ptr<ImGuiFrame> m_Frame = nullptr;
    };
} // namespace Nexus::UI