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
        virtual void *GetHandle() override;

    private:
        unsigned int m_Handle;
    };
}