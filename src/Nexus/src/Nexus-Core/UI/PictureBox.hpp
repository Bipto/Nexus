#pragma once

#include "Control.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::UI
{
class PictureBox : public Control
{
  public:
    virtual void OnUpdate() override;
    virtual void OnRender(Nexus::Graphics::BatchRenderer *renderer) override;
    virtual void OnAutoSize() override;
    virtual void HandleMouseClick(const MouseClick &e) override;

    void SetTexture(Ref<Graphics::Texture2D> texture);
    const Ref<Graphics::Texture2D> GetTexture() const;

  private:
    Ref<Graphics::Texture2D> m_Texture = nullptr;
};
} // namespace Nexus::UI