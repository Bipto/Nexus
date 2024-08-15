#pragma once

#include "Control.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::UI
{
class Label : public Control
{
  public:
    virtual void OnUpdate() override;
    virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) override;
    virtual void OnAutoSize() override;
    virtual void HandleMouseClick(const MouseClick &e) override;

    void SetText(const std::string &text);
    const std::string &GetText() const;

  private:
    std::string m_Text;
};
} // namespace Nexus::UI