#pragma once

namespace Nexus::UI
{
    class IButton
    {
      public:
        virtual ~IButton() = default;
        virtual void OnClick(std::function<void()> handler) = 0;
    };
} // namespace Nexus::UI