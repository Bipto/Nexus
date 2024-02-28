#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureOpenGL : public Texture
    {
    public:
        TextureOpenGL(const TextureSpecification &spec);
        ~TextureOpenGL();
        virtual void SetData(const void *data, uint32_t size) override;

        unsigned int GetNativeHandle();
        GLenum GetTextureType();

    private:
        unsigned int m_Handle;
        GLenum m_DataFormat = 0;
        GLenum m_InternalFormat = 0;
        GLenum m_BaseType = 0;
        GLenum m_TextureType = 0;
    };
}

#endif