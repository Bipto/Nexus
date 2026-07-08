#pragma once

#include <memory>

#include "UI/Menubar.hpp"
#include "UI/StatusBar.hpp"

namespace Nexus::UI
{
    class IFrame
    {
      public:
        virtual ~IFrame() = default;
        virtual IMenubar *CreateMenubar() = 0;
        virtual IStatusBar *CreateStatusbar() = 0;
    };
} // namespace Nexus::UI