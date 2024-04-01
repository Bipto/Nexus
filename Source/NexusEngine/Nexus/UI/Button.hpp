#pragma once

#include "Control.hpp"

#include <string>

namespace Nexus::UI
{
    class Button : public Control
    {
    public:
        virtual void Update() override;
        virtual void Render(Graphics::BatchRenderer *batchRenderer) override;

        void SetText(const std::string &text);
        const std::string &GetText() const;

    private:
        std::string m_Text;
    };
}