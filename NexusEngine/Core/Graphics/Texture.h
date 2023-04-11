#pragma once

#include "stb_image.h"
#include "TextureFormat.h"

namespace Nexus
{
    enum SamplerState
    {
        LinearClamp,
        LinearWrap,
        PointClamp,
        PointWrap
    };

    struct TextureSpecification
    {
        uint32_t Width;
        uint32_t Height;
        TextureFormat Format;
        SamplerState SamplerState;
        bool RetainImageData = true;
    };

    class Texture
    {
        public:
            Texture(const char* filepath){}
            virtual void Bind(unsigned int slot = 0) = 0;
            virtual void* GetHandle() = 0;
        protected:
            int m_Width;
            int m_Height;
            int m_NumOfChannels;
            unsigned char* m_Data;
    };
}