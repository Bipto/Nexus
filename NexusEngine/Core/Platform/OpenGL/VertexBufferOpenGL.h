#pragma once

#include "GL.h"
#include "../../Graphics/VertexBuffer.h"

namespace NexusEngine
{
    class VertexBufferOpenGL : public VertexBuffer
    {
        public:
            VertexBufferOpenGL(float vertices[], unsigned int size, unsigned int vertexStride, unsigned int vertexOffset, unsigned int vertexCount) 
                : VertexBuffer(vertices, size, vertexStride, vertexOffset, vertexCount)
            {
                glGenBuffers(1, &this->m_VBO);
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
                glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
            }
            void Bind() override 
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
            }
        private:
            unsigned int m_VBO;
    };
}