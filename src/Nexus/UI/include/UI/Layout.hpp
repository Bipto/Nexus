#pragma once

#include <string>
#include <string_view>

#include "UI/Frame.hpp"
#include "UI/Panel.hpp"
#include "UI/StatusBar.hpp"

namespace Nexus::UI
{
    class ILayout
    {
      public:
        virtual ~ILayout() = default;
        virtual std::unique_ptr<IPanel> CreatePanel() = 0;
        virtual IFrame *CreateFrame(const std::string &title) = 0;

        virtual void LogFatal(const std::string &message) = 0;
        virtual void LogError(const std::string &message) = 0;
        virtual void LogWarning(const std::string &message) = 0;
        virtual void LogMessage(const std::string &message) = 0;
        virtual void LogInfo(const std::string &message) = 0;
        virtual void ShowMessageBox(const std::string &title, const std::string &message) = 0;
    };
} // namespace Nexus::UI