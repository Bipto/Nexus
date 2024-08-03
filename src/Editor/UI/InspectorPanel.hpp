#pragma once

#include "Nexus-Core/Runtime/Entity.hpp"
#include "Panel.hpp"

namespace Editor
{
    class Inspector : public Panel
    {
    public:
        virtual void OnLoad() override;
        virtual void OnRender() override;
    };
}