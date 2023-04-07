#include "TextureOpenGL.h"

namespace Nexus
{
    TextureOpenGL::TextureOpenGL(const char* filepath) : Texture(filepath)
    {
        this->m_Data = stbi_load(filepath, &m_Width, &m_Height, &m_NumOfChannels, 0);

        glGenTextures(1, &this->m_Handle);
        glBindTexture(GL_TEXTURE_2D, this->m_Handle);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->m_Width, this->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->m_Data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(this->m_Data);
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

    void* TextureOpenGL::GetHandle()
    {
        return (void*)m_Handle;   
    }
}