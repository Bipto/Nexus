#include "Texture.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics {
Texture2D::Texture2D(const Texture2DSpecification &spec, GraphicsDevice *device)
    : m_Specification(spec), m_Device(device) {}

Texture2D::~Texture2D() {}

const Texture2DSpecification &Texture2D::GetSpecification() {
  return m_Specification;
}

uint32_t Texture2D::GetLevels() const { return m_Specification.MipLevels; }

Cubemap::Cubemap(const CubemapSpecification &spec, GraphicsDevice *device)
    : m_Specification(spec), m_Device(device) {}

Cubemap::~Cubemap() {}

const CubemapSpecification &Cubemap::GetSpecification() const {
  return m_Specification;
}

uint32_t Cubemap::GetLevels() const { return m_Specification.MipLevels; }

} // namespace Nexus::Graphics
