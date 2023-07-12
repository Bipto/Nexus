#pragma once

#include "GL.hpp"
#include "Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureOpenGL : public Texture
    {
    public:
        TextureOpenGL(TextureSpecification spec);
        ~TextureOpenGL();
        void Bind(unsigned int slot = 0);
        virtual void *GetHandle() override;

    private:
        unsigned int m_Handle;
    };
}