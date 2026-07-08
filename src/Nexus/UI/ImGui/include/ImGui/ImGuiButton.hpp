#pragma once

#include <functional>
#include <optional>
#include <string>

#include "UI/Button.hpp"

#include "ImGui/ImGuiControl.hpp"

namespace Nexus::UI
{
    class ImGuiButton final : public IButton, public ImGuiControl
    {
      public:
        ImGuiButton(
            std::string_view text, std::optional<Position> position,
            std::optional<Size> size
        );
        ~ImGuiButton() final = default;

        void Render() final;

        void OnClick(std::function<void()> handler) final;

      private:
        std::string m_Text = {};
        std::function<void()> m_OnClick = {};

        std::optional<Position> m_Position = {};
        std::optional<Size> m_Size = {};
    };
} // namespace Nexus::UI