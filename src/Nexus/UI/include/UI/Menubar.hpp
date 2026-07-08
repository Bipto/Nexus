#pragma once

#include <functional>
#include <string>

#include "UI/Control.hpp"

namespace Nexus::UI
{
    class IMenuBase
    {
      public:
        virtual ~IMenuBase() = default;
    };

    class IMenuItem : public IMenuBase
    {
      public:
        virtual ~IMenuItem() = default;
        virtual void OnClick(std::function<void()> handler) = 0;
    };

    class ITextMenuItem : public IMenuItem
    {
      public:
        virtual ~ITextMenuItem() = default;
    };

    class ISeparatorMenuItem : public IMenuItem
    {
      public:
        virtual ~ISeparatorMenuItem() = default;
    };

    class IMenu : public IMenuBase
    {
      public:
        virtual ~IMenu() = default;
        virtual IMenuItem *Append(const std::string &text) = 0;
        virtual IMenu *AppendSubMenu(const std::string &text) = 0;
        virtual IMenuItem *AppendSeparator() = 0;
        virtual void OnMenuOpened(std::function<void()> handler) = 0;
        virtual void OnMenuClosed(std::function<void()> handler) = 0;
    };

    class IMenubar
    {
      public:
        virtual ~IMenubar() = default;
        virtual IMenu *CreateMenu(const std::string &text) = 0;
    };
} // namespace Nexus::UI