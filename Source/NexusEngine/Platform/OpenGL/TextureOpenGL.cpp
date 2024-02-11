#if defined(NX_PLATFORM_OPENGL)

#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureSpecification &spec)
        : Texture(spec)
    {
        glGenTextures(1, &this->m_Handle);
        glBindTexture(GL_TEXTURE_2D, this->m_Handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        m_SamplerMode = GL::GetSamplerState(spec.SamplerState);
        m_WrapMode = GL::GetWrapMode(spec.SamplerState);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_SamplerMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_SamplerMode);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapMode);

        m_TextureFormat = GL::GetColorTextureFormat(spec.Format);
        glTexStorage2D(GL_TEXTURE_2D, 1, m_TextureFormat, spec.Width, spec.Height);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    TextureOpenGL::~TextureOpenGL()
    {
        glDeleteTextures(1, &this->m_Handle);
    }

    void TextureOpenGL::Bind(unsigned int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, this->m_Handle);
    }

    ResourceHandle TextureOpenGL::GetHandle()
    {
        return (ResourceHandle)m_Handle;
    }

    void TextureOpenGL::SetData(const void *data, uint32_t size)
    {
        GL::ClearErrors();
        glBindTexture(GL_TEXTURE_2D, m_Handle);

        GLenum internalFormat = GL::GetInternalTextureFormat(m_Specification.Format);
        GLenum baseType = GL::GetTextureFormatBaseType(m_Specification.Format);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Specification.Width, m_Specification.Height, internalFormat, baseType, data);
        GL::CheckErrors();
    }
}

#endif