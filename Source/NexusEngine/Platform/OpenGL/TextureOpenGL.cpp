#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    TextureOpenGL::TextureOpenGL(const TextureSpecification &spec) : Texture(spec)
    {
        glGenTextures(1, &this->m_Handle);
        glBindTexture(GL_TEXTURE_2D, this->m_Handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        auto samplerMode = GL::GetSamplerState(spec.SamplerState);
        auto wrapMode = GL::GetWrapMode(spec.SamplerState);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplerMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplerMode);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

        auto textureFormat = GL::GetColorTextureFormat(spec.Format);
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, spec.Width, spec.Height, 0, textureFormat, GL_UNSIGNED_BYTE, spec.Data);
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

    ResourceHandle TextureOpenGL::GetHandle()
    {
        return (ResourceHandle)m_Handle;
    }
}