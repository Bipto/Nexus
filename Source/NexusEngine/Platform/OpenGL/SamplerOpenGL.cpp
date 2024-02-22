#include "SamplerOpenGL.hpp"

#include "GL.hpp"

#include "Nexus/Utils/Utils.hpp"

namespace Nexus::Graphics
{
        SamplerOpenGL::SamplerOpenGL(const SamplerSpecification &spec)
            : m_Specification(spec)
        {
                glGenSamplers(1, &m_Sampler);

                GLenum min, max;
                GL::GetSamplerFilter(m_Specification.SampleFilter, min, max);

                // texture sampling options
                glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, min);
                glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, max);
                glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(m_Specification.AddressModeU));
                glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(m_Specification.AddressModeV));
                glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(m_Specification.AddressModeW));

// texture anisotropy
#if !defined(NX_PLATFORM_WEBGL2)
                glSamplerParameterf(m_Sampler, GL_MAX_TEXTURE_MAX_ANISOTROPY, m_Specification.MaximumAnisotropy);
#else
                glSamplerParameterf(m_Sampler, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, m_Specification.MaximumAnisotropy);
#endif

                const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(spec.BorderColor);

                // border colour
                GLfloat border[] = {color.r,
                                    color.g,
                                    color.b,
                                    color.a};

#if !defined(NX_PLATFORM_WEBGL2)
                glSamplerParameterfv(m_Sampler, GL_TEXTURE_BORDER_COLOR, border);
#else
                glSamplerParameterfv(m_Sampler, GL_TEXTURE_BORDER_COLOR_EXT, border);
#endif
                // LOD
                glSamplerParameterf(m_Sampler, GL_TEXTURE_MIN_LOD, m_Specification.MinimumLOD);
                glSamplerParameterf(m_Sampler, GL_TEXTURE_MAX_LOD, m_Specification.MaximumLOD);

#if !defined(NX_PLATFORM_WEBGL2)
                glSamplerParameterf(m_Sampler, GL_TEXTURE_LOD_BIAS, m_Specification.LODBias);
#endif

                // texture comparison
                auto comparisonFunction = GL::GetComparisonFunction(m_Specification.ComparisonFunction);
                glSamplerParameteri(m_Sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glSamplerParameteri(m_Sampler, GL_TEXTURE_COMPARE_FUNC, comparisonFunction);
        }

        SamplerOpenGL::~SamplerOpenGL()
        {
                glDeleteSamplers(1, &m_Sampler);
        }

        const SamplerSpecification &SamplerOpenGL::GetSamplerSpecification()
        {
                return m_Specification;
        }

        unsigned int SamplerOpenGL::GetHandle() const
        {
                return m_Sampler;
        }
}
