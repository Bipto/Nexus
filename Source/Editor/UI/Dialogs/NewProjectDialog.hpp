#pragma once

#include "UI/Panel.hpp"

namespace Editor
{
    class NewProjectDialog : public Panel
    {
        virtual void OnLoad() override;
        virtual void OnRender() override;
    };
}