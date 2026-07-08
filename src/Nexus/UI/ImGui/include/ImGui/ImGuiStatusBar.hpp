#pragma once

#include <string_view>

#include "UI/StatusBar.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "ImGui/ImGuiControl.hpp"

namespace Nexus::UI
{
    class ImGuiStatusBar final : public IStatusBar, public ImGuiControl
    {
      public:
        ImGuiStatusBar() = default;
        ~ImGuiStatusBar() final = default;
        void SetStatusText(std::string_view text) final;
        void SetHelpText(std::string_view text) final;

        void Render() final;

      private:
        std::string m_StatusText = {};
        std::string m_HelpText = {};
    };
} // namespace Nexus::UI