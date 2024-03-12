#if defined(NX_PLATFORM_OPENGL)

#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureSpecification &spec)
        : Texture(spec)
    {
        m_DataFormat = GL::GetPixelDataFormat(spec.Format);
        m_InternalFormat = GL::GetSizedInternalFormat(spec.Format, false);
        m_BaseType = GL::GetPixelType(spec.Format);

        glGenTextures(1, &m_Handle);
#if defined(NX_PLATFORM_GL_DESKTOP)
        if (spec.Samples != SampleCount::SampleCount1)
        {
            uint32_t samples = GetSampleCount(spec.Samples);
            m_TextureType = GL_TEXTURE_2D_MULTISAMPLE;
            glBindTexture(m_TextureType, m_Handle);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, m_InternalFormat, m_Specification.Width, m_Specification.Height, GL_FALSE);
        }
        else
        {
#endif
            // if we do not support multisampling, then we force this to 1
            m_Specification.Samples = SampleCount::SampleCount1;
            m_TextureType = GL_TEXTURE_2D;
            glBindTexture(m_TextureType, m_Handle);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexStorage2D(m_TextureType, 1, m_InternalFormat, spec.Width, spec.Height);
#if defined(NX_PLATFORM_GL_DESKTOP)
        }
#endif
    }

    TextureOpenGL::~TextureOpenGL()
    {
        glDeleteTextures(1, &this->m_Handle);
    }

    void TextureOpenGL::SetData(const void *data, uint32_t size)
    {
        GL::ClearErrors();
        glBindTexture(m_TextureType, m_Handle);

        glTexSubImage2D(m_TextureType, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat, m_BaseType, data);
        GL::CheckErrors();
    }

    unsigned int TextureOpenGL::GetNativeHandle()
    {
        return m_Handle;
    }

    GLenum TextureOpenGL::GetTextureType()
    {
        return m_TextureType;
    }
}

#endif