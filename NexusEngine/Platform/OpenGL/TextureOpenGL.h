#pragma once

#include "GL.h"
#include "Core/Graphics/Texture.h"

namespace Nexus
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