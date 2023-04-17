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
            virtual void Bind(unsigned int slot = 0) override;
            virtual void* GetHandle() override;
        private:
            unsigned int m_Handle;
    };
}