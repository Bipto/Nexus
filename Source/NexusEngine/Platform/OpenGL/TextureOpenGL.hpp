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
        virtual void SetData(const void *data, uint32_t size, uint32_t level) override;
        virtual std::vector<std::byte> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        unsigned int GetNativeHandle();
        GLenum GetTextureType();
        GLenum GetDataFormat();
        GLenum GetPixelType();

    private:
        unsigned int m_Handle;
        GLenum m_DataFormat = 0;
        GLenum m_InternalFormat = 0;
        GLenum m_BaseType = 0;
        GLenum m_TextureType = 0;
    };
}

#endif