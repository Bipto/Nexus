#pragma once

namespace NexusEngine
{
    class IndexBuffer
    {
        public:
            IndexBuffer(unsigned int indices[], unsigned int size){}
            IndexBuffer(const IndexBuffer&) = delete;
            virtual void Bind() = 0;
    };
}