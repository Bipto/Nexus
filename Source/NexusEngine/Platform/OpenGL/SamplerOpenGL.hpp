#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/Sampler.hpp"

namespace Nexus::Graphics
{
    class SamplerOpenGL : public Sampler
    {
    public:
        SamplerOpenGL(const SamplerSpecification &spec);
        virtual ~SamplerOpenGL();
        virtual const SamplerSpecification &GetSamplerSpecification() override;
        unsigned int GetHandle() const;

    private:
        SamplerSpecification m_Specification;
        unsigned int m_Sampler = 0;
    };
}

#endif
