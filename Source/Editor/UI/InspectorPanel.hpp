#pragma once

#include "Nexus/Runtime/Entity.hpp"
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