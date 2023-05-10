#include "ShaderOpenGL.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    GLenum GetGLBaseType(const BufferElement element)
    {
        switch (element.Type)
        {
            case ShaderDataType::Float:     return GL_FLOAT;
            case ShaderDataType::Float2:    return GL_FLOAT;
            case ShaderDataType::Float3:    return GL_FLOAT;
            case ShaderDataType::Float4:    return GL_FLOAT;
            case ShaderDataType::Int:       return GL_INT;
            case ShaderDataType::Int2:      return GL_INT;
            case ShaderDataType::Int3:      return GL_INT;
            case ShaderDataType::Int4:      return GL_INT;
        }
    }

    ShaderOpenGL::ShaderOpenGL(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
    {
        Compile(vertexShaderSource, fragmentShaderSource);
        m_Layout = layout;

        m_VertexShaderSource = vertexShaderSource;
        m_FragmentShaderSource = fragmentShaderSource;
    }

    void ShaderOpenGL::Bind() 
    {
        glUseProgram(this->m_ProgramHandle);
        SetLayout();
    }

    void ShaderOpenGL::SetShaderUniform1i(const std::string& name, int value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniform1f(loc, value);
    }

    void ShaderOpenGL::SetShaderUniform1f(const std::string& name, float value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniform1f(loc, value);
    }

    void ShaderOpenGL::SetShaderUniform2f(const std::string& name, const glm::vec2& value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniform2f(loc, value.x, value.y);
    }

    void ShaderOpenGL::SetShaderUniform3f(const std::string& name, const glm::vec3& value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniform3f(loc, value.x, value.y, value.z);
    }

    void ShaderOpenGL::SetShaderUniform4f(const std::string& name, const glm::vec4& value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniform4f(loc, value.x, value.y, value.z, value.w);
    }

    void ShaderOpenGL::SetShaderUniformMat3(const std::string& name, const glm::mat3& value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderOpenGL::SetShaderUniformMat4(const std::string& name, const glm::mat4& value)
    {
        unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderOpenGL::SetTexture(Ref<Texture> texture, int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, (unsigned int)texture->GetHandle());
    }

    const std::string &ShaderOpenGL::GetVertexShaderSource()
    {
        return m_VertexShaderSource;
    }

    const std::string &ShaderOpenGL::GetFragmentShaderSource()
    {
        return m_FragmentShaderSource;
    }

    const BufferLayout& ShaderOpenGL::GetLayout() const
    { 
        return m_Layout; 
    }

    void ShaderOpenGL::Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
    {
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexShaderChar = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderChar, NULL);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::string errorMessage = "Error: Vertex Shader - " + std::string(infoLog);
            NX_ERROR(errorMessage);
        }

        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentShaderChar = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderChar, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::string errorMessage = "Error: Pixel Shader - " + std::string(infoLog);
            NX_ERROR(errorMessage);
        }

        this->m_ProgramHandle = glCreateProgram();
        glAttachShader(this->m_ProgramHandle, vertexShader);
        glAttachShader(this->m_ProgramHandle, fragmentShader);
        glLinkProgram(this->m_ProgramHandle);

        glGetProgramiv(this->m_ProgramHandle, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->m_ProgramHandle, 512, NULL, infoLog);
            std::string errorMessage = "Error: Shader Program - " + std::string(infoLog);
            NX_ERROR(errorMessage);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void ShaderOpenGL::SetLayout()
    {
        int index = 0;
        for (auto& element : m_Layout)
        {
            glVertexAttribPointer(index,
                element.GetComponentCount(),
                GetGLBaseType(element),
                element.Normalized ? GL_TRUE : GL_FALSE,
                m_Layout.GetStride(),
                (void*)element.Offset);
            
            glEnableVertexAttribArray(index);
            index++;
        }
    }
}