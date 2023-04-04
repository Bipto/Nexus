#pragma once

#include "GL.h"
#include "Core/Graphics/Buffer.h"

namespace Nexus
{
    class VertexBufferOpenGL : public VertexBuffer
    {
        public:
            VertexBufferOpenGL(const std::vector<float>& vertices) 
                : VertexBuffer(vertices)
            {
                glGenBuffers(1, &this->m_VBO);
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

                m_VertexCount = vertices.size();
            }
            void Bind() 
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);                
            }
        
            virtual unsigned int GetVertexCount() override { return m_VertexCount; }
           
        private:
            unsigned int m_VBO = 0;
            unsigned int m_VertexCount = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
        public:
            IndexBufferOpenGL(unsigned int indices[], unsigned int size)
                : IndexBuffer(indices, size)
            {
                glGenBuffers(1, &this->m_IBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
            
                m_IndexCount = size;
            }
            void Bind()
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
            }

            virtual unsigned int GetIndexCount() { return m_IndexCount; }
        private:
           unsigned int m_IBO; 
           unsigned int m_IndexCount;
    };
}