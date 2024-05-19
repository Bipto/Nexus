#pragma once

#include "Control.hpp"

#include <vector>

namespace Nexus::UI
{
    class Panel : public Control
    {
    public:
        virtual ~Panel();

        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;
        virtual void Update() override;
        void AddControl(Control *control);
        void RemoveControl(Control *control);

    private:
        std::vector<Control *> m_Controls;
    };
}