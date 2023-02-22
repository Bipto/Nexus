#pragma once

#include "GL.h"
#include "Core/Graphics/Buffer.h"

namespace Nexus
{
    static GLenum GetBaseType(const BufferElement element)
    {
        switch (element.Type)
        {
            case ShaderDataType::Float:     return GL_FLOAT;
            case ShaderDataType::Float2:    return GL_FLOAT;
            case ShaderDataType::Float3:    return GL_FLOAT;
            case ShaderDataType::Float4:    return GL_FLOAT;
            case ShaderDataType::Mat3:      return GL_FLOAT;
            case ShaderDataType::Mat4:      return GL_FLOAT;
            case ShaderDataType::Int:       return GL_INT;
            case ShaderDataType::Int2:      return GL_INT;
            case ShaderDataType::Int3:      return GL_INT;
            case ShaderDataType::Int4:      return GL_INT;
            case ShaderDataType::Bool:      return GL_BOOL;
        }
    }

    class VertexBufferOpenGL : public VertexBuffer
    {
        public:
            VertexBufferOpenGL(const std::vector<float>& vertices, const BufferLayout& layout) 
                : VertexBuffer(vertices, layout)
            {
                glGenBuffers(1, &this->m_VBO);
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

                //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                SetLayout(layout);
            }
            virtual void Bind() override 
            {
                glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);                
            }
            virtual void SetLayout(const BufferLayout& layout) override
            {
                /* glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
                glEnableVertexAttribArray(0);
                
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vertex::Position));
                glEnableVertexAttribArray(1); */

                m_Layout = layout;

                int index = 0;
                for (auto& element : m_Layout)
                {
                    glVertexAttribPointer(index,
                        element.GetComponentCount(),
                        GetBaseType(element),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        m_Layout.GetStride(),
                        (void*)element.Offset);
                    
                    glEnableVertexAttribArray(index);
                    index++;
                }
            }

            virtual const BufferLayout& GetLayout() const { return m_Layout; }
        private:
            unsigned int m_VBO;
            BufferLayout m_Layout;
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