#pragma once

#include <string>
#include <string_view>

namespace Nexus::UI
{
    class IStatusBar
    {
      public:
        virtual ~IStatusBar() = default;
        virtual void SetStatusText(std::string_view text) = 0;
        virtual void SetHelpText(std::string_view text) = 0;
    };
} // namespace Nexus::UI