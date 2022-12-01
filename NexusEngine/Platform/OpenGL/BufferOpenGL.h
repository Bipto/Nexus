#pragma once

#include "GL.h"
#include "../../Core/Graphics/Buffer.h"

namespace Nexus
{
    class VertexBufferOpenGL : public VertexBuffer
    {
        public:
            VertexBufferOpenGL(const std::vector<Vertex>& vertices) 
                : VertexBuffer(vertices)
            {
                glGenBuffers(1, &this->m_VBO);
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

                //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            }
            void Bind() override 
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
                glEnableVertexAttribArray(0);
                
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vertex::Position));
                glEnableVertexAttribArray(1);
            }
        private:
            unsigned int m_VBO;
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
            }
            void Bind() override
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
            }
        private:
           unsigned int m_IBO; 
    };
}