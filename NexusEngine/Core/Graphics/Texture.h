#pragma once

#include "stb_image.h"
#include <memory>

namespace NexusEngine
{
    class Texture
    {
        public:
            Texture(const char* filepath){}
        protected:
            int m_Width;
            int m_Height;
            int m_NumOfChannels;
    };
}