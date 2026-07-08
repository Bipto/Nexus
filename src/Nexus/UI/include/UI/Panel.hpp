#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "UI/Button.hpp"
#include "UI/Control.hpp"
#include "UI/Menubar.hpp"

namespace Nexus::UI
{
    class IPanel
    {
      public:
        virtual ~IPanel() = default;
        virtual IButton *CreateButton(std::string_view text) = 0;
    };
} // namespace Nexus::UI