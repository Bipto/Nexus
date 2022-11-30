#pragma once

#include "../../Core/Graphics/Texture.h"
#include "glad/glad.h"

namespace NexusEngine
{
    class TextureOpenGL : public Texture
    {
        public:
            TextureOpenGL(const char* filepath) : Texture(filepath)
            {
                unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_NumOfChannels, 0);

                glGenTextures(1, &this->m_Handle);
                glBindTexture(GL_TEXTURE_2D, this->m_Handle);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->m_Width, this->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
            }
        private:
            unsigned int m_Handle;
    };
}