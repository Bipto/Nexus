#pragma once

#include "Control.hpp"

#include <string>

namespace Nexus::UI
{
    class Label : public Control
    {
    public:
        virtual void OnUpdate() override;
        virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) override;

        void SetText(const std::string &text);
        const std::string &GetText() const;

    private:
        std::string m_Text;
    };
}