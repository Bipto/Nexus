#pragma once

#include "GL.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureOpenGL : public Texture
    {
    public:
        TextureOpenGL(const TextureSpecification &spec);
        ~TextureOpenGL();
        void Bind(unsigned int slot = 0);
        virtual ResourceHandle GetHandle() override;
        virtual void SetData(const void *data, uint32_t size) override;

    private:
        unsigned int m_Handle;
        GLenum m_TextureFormat = 0;
        GLenum m_SamplerMode = 0;
        GLenum m_WrapMode = 0;
    };
}