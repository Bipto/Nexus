#if defined(NX_PLATFORM_OPENGL)

#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureSpecification &spec)
        : Texture(spec)
    {
        glGenTextures(1, &m_Handle);
        glBindTexture(GL_TEXTURE_2D, m_Handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        m_DataFormat = GL::GetPixelDataFormat(spec.Format);
        m_InternalFormat = GL::GetSizedInternalFormat(spec.Format, false);
        m_BaseType = GL::GetPixelType(spec.Format);

        glTexStorage2D(GL_TEXTURE_2D, 1, m_InternalFormat, spec.Width, spec.Height);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    TextureOpenGL::~TextureOpenGL()
    {
        glDeleteTextures(1, &this->m_Handle);
    }

    ResourceHandle TextureOpenGL::GetHandle()
    {
        return (ResourceHandle)m_Handle;
    }

    void TextureOpenGL::SetData(const void *data, uint32_t size)
    {
        GL::ClearErrors();
        glBindTexture(GL_TEXTURE_2D, m_Handle);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat, m_BaseType, data);
        GL::CheckErrors();
    }

    unsigned int TextureOpenGL::GetNativeHandle()
    {
        return m_Handle;
    }
}

#endif