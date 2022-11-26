#pragma once

#include "GL.h"
#include "../../Core/Graphics/IndexBuffer.h"

namespace NexusEngine
{
    class IndexBufferOpenGL : public IndexBuffer
    {
        public:
            IndexBufferOpenGL(unsigned int indices[], unsigned int size)
                : IndexBuffer(indices, size)
            {
                glGenBuffers(1, &this->m_IBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
            }
            void Bind() override
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
            }
        private:
           unsigned int m_IBO; 
    };
}