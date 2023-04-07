#pragma once

#include "GL.h"
#include "Core/Graphics/Buffer.h"

namespace Nexus
{
    class VertexBufferOpenGL : public VertexBuffer
    {
        public:
            VertexBufferOpenGL(const std::vector<float>& vertices);
            void Bind();        
            virtual unsigned int GetVertexCount() override { return m_VertexCount; }
           
        private:
            unsigned int m_VBO = 0;
            unsigned int m_VertexCount = 0;
    };

    class IndexBufferOpenGL : public IndexBuffer
    {
        public:
            IndexBufferOpenGL(const std::vector<unsigned int>& indices);
            void Bind();
            virtual unsigned int GetIndexCount() { return m_IndexCount; }
        private:
           unsigned int m_IBO; 
           unsigned int m_IndexCount;
    };
}