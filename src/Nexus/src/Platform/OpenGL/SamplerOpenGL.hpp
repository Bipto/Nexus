#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/Graphics/Sampler.hpp"

namespace Nexus::Graphics
{
class SamplerOpenGL : public Sampler
{
  public:
    SamplerOpenGL(const SamplerSpecification &spec);
    virtual ~SamplerOpenGL();
    virtual const SamplerSpecification &GetSamplerSpecification() override;
    unsigned int GetHandle() const;

    void Bind(uint32_t slot, bool hasMips);
    void Setup(bool hasMips);

  private:
    SamplerSpecification m_Specification;
    unsigned int m_Handle = 0;
};
} // namespace Nexus::Graphics

#endif
