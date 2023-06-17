#pragma once

#include "AudioBuffer.h"

namespace Nexus
{
    class AudioBuffer
    {
    public:
        virtual void *GetHandle() = 0;
    };
}