#if defined(NX_PLATFORM_OPENGL)

#include "SamplerOpenGL.hpp"

#include "GL.hpp"

#include "Nexus/Utils/Utils.hpp"

namespace Nexus::Graphics
{
        SamplerOpenGL::SamplerOpenGL(const SamplerSpecification &spec)
            : m_Specification(spec)
        {
                glCall(glGenSamplers(1, &m_Sampler));

                uint32_t levels = spec.MaximumLOD - spec.MinimumLOD;

                GLenum min, max;
                GL::GetSamplerFilter(m_Specification.SampleFilter, min, max, levels > 1);

                // texture sampling options
                glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, min));
                glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, max));
                glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(m_Specification.AddressModeU)));
                glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(m_Specification.AddressModeV)));
                glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(m_Specification.AddressModeW)));

                // texture anisotropy
                glCall(glSamplerParameterf(m_Sampler, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, m_Specification.MaximumAnisotropy));

                const glm::vec4 color = Nexus::Utils::ColorFromBorderColor(spec.TextureBorderColor);

                // border colour
                GLfloat border[] = {color.r,
                                    color.g,
                                    color.b,
                                    color.a};

                glCall(glSamplerParameterfv(m_Sampler, GL_TEXTURE_BORDER_COLOR_EXT, border));
                // LOD
                glCall(glSamplerParameterf(m_Sampler, GL_TEXTURE_MIN_LOD, m_Specification.MinimumLOD));
                glCall(glSamplerParameterf(m_Sampler, GL_TEXTURE_MAX_LOD, m_Specification.MaximumLOD));

#if defined(NX_PLATFORM_GL_DESKTOP)
                glCall(glSamplerParameterf(m_Sampler, GL_TEXTURE_LOD_BIAS_EXT, m_Specification.LODBias));
#endif

                // texture comparison
                if (m_Specification.SamplerComparisonFunction != ComparisonFunction::Never)
                {
                        auto comparisonFunction = GL::GetComparisonFunction(m_Specification.SamplerComparisonFunction);
                        glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
                        glCall(glSamplerParameteri(m_Sampler, GL_TEXTURE_COMPARE_FUNC, comparisonFunction));
                }
        }

        SamplerOpenGL::~SamplerOpenGL()
        {
                glCall(glDeleteSamplers(1, &m_Sampler));
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

#endif
