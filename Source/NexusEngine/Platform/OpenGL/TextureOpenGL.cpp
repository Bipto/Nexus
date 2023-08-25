#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureSpecification &spec) : Texture(spec)
    {
        glGenTextures(1, &this->m_Handle);
        glBindTexture(GL_TEXTURE_2D, this->m_Handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spec.Width, spec.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spec.Data);
        glGenerateMipmap(GL_TEXTURE_2D);

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

    void *TextureOpenGL::GetHandle()
    {
        return (void *)m_Handle;
    }
}