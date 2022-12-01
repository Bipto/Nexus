#pragma once

#include "stb_image.h"
#include <memory>

namespace NexusEngine
{
    class Texture
    {
        public:
            Texture(const char* filepath){}
            virtual void Bind(unsigned int slot = 0) = 0;
        protected:
            int m_Width;
            int m_Height;
            int m_NumOfChannels;
            unsigned char* m_Data;
    };
}