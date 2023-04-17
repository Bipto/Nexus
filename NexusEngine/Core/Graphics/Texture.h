#pragma once

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
        int32_t Width;
        int32_t Height;
        int32_t NumberOfChannels;
        TextureFormat Format;
        SamplerState SamplerState;
        unsigned char* Data;
        bool RetainImageData = true;
    };

    class Texture
    {
        public:
            Texture(TextureSpecification spec){}
            virtual void Bind(unsigned int slot = 0) = 0;
            virtual void* GetHandle() = 0;
        protected:
            int m_Width;
            int m_Height;
            int m_NumOfChannels;
            unsigned char* m_Data;
    };
}