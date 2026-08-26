#pragma once

#include <memory>
#include <vector>

#include "UI/Control.hpp"

namespace Nexus::UI
{
    class ImGuiControl
    {
      public:
        virtual ~ImGuiControl() = default;
        virtual void Render() = 0;

      protected:
        template <typename InstantiateType, typename ReturnType, typename... Args>
        ReturnType *AddChild(Args &&...args)
        {
            static_assert(std::derived_from<InstantiateType, ReturnType>,
                          "InstantiateType must derive from ReturnType");

            std::unique_ptr<InstantiateType> control = std::make_unique<InstantiateType>(std::forward<Args>(args)...);
            ReturnType *returnValue = control.get();
            m_Children.push_back(std::move(control));
            return returnValue;
        }

      protected:
        std::vector<std::unique_ptr<ImGuiControl>> m_Children = {};
    };
} // namespace Nexus::UI