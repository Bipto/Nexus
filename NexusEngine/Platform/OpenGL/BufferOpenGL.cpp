#include "BufferOpenGL.h"
#include "Core/Logging/Log.h"

namespace Nexus
{
    VertexBufferOpenGL::VertexBufferOpenGL(const std::vector<float>& vertices) 
        : VertexBuffer(vertices)
    {
        GL::ClearErrors();

        glGenBuffers(1, &this->m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        GL::CheckErrors();

        m_VertexCount = vertices.size();
    }

    void VertexBufferOpenGL::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);    
    }

    IndexBufferOpenGL::IndexBufferOpenGL(const std::vector<unsigned int>& indices)
        : IndexBuffer(indices)
    {
        GL::ClearErrors();

        glGenBuffers(1, &this->m_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
        size_t size = indices.size() * sizeof(unsigned int);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices.data(), GL_STATIC_DRAW);
    
        GL::CheckErrors();

        m_IndexCount = indices.size();
    }

    void IndexBufferOpenGL::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
    }

    UniformBufferOpenGL::UniformBufferOpenGL(const UniformResourceBinding& binding)
        : UniformBuffer(binding)
    {
        GL::ClearErrors();

        m_Binding = binding;

        #ifndef __EMSCRIPTEN__
        glCreateBuffers(1, &m_UBO);
        glNamedBufferData(m_UBO, m_Binding.Size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding.Binding, m_UBO);
        #else
        glGenBuffers(1, &m_UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
        glBufferData(GL_UNIFORM_BUFFER, binding.Size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        #endif     

        GL::CheckErrors();
    }

    UniformBufferOpenGL::~UniformBufferOpenGL()
    {
        GL::ClearErrors();
        glDeleteBuffers(1, &m_UBO);
        GL::CheckErrors();
    }

    void UniformBufferOpenGL::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        GL::ClearErrors();
        #ifndef __EMSCRIPTEN__
        glNamedBufferSubData(m_UBO, offset, size, data);
        #else
        glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        #endif        
        GL::CheckErrors();
    }

    void UniformBufferOpenGL::BindToShader(Ref<Shader> shader)
    {
        GL::ClearErrors();
        Ref<ShaderOpenGL> glShader = std::dynamic_pointer_cast<ShaderOpenGL>(shader);
        unsigned int index = glGetUniformBlockIndex(glShader->GetHandle(), m_Binding.Name.c_str());
        glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
        glUniformBlockBinding(glShader->GetHandle(), index, m_Binding.Binding);
        glBindBufferRange(GL_UNIFORM_BUFFER, m_Binding.Binding, m_UBO, 0, m_Binding.Size);
        GL::CheckErrors();
    }
}