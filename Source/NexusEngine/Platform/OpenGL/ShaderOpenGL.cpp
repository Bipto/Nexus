#include "ShaderOpenGL.hpp"

#include "Nexus/Logging/Log.hpp"

#include "BufferOpenGL.hpp"

namespace Nexus::Graphics
{
    ShaderOpenGL::ShaderOpenGL(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        Compile(vertexShaderSource, fragmentShaderSource);
        m_Layout = layout;

        m_VertexShaderSource = vertexShaderSource;
        m_FragmentShaderSource = fragmentShaderSource;
    }

    ShaderOpenGL::~ShaderOpenGL()
    {
        glDeleteProgram(m_ProgramHandle);
    }

    void ShaderOpenGL::Bind()
    {
        glUseProgram(this->m_ProgramHandle);
    }

    const std::string &ShaderOpenGL::GetVertexShaderSource()
    {
        return m_VertexShaderSource;
    }

    const std::string &ShaderOpenGL::GetFragmentShaderSource()
    {
        return m_FragmentShaderSource;
    }

    const VertexBufferLayout &ShaderOpenGL::GetLayout() const
    {
        return m_Layout;
    }

    void ShaderOpenGL::Compile(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
    {
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char *vertexShaderChar = vertexShaderSource.c_str();
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
        const char *fragmentShaderChar = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderChar, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::string errorMessage = "Error: Pixel Shader - " + std::string(infoLog);
            NX_ERROR(errorMessage);
        }

        m_ProgramHandle = glCreateProgram();
        glAttachShader(m_ProgramHandle, vertexShader);
        glAttachShader(m_ProgramHandle, fragmentShader);
        glLinkProgram(m_ProgramHandle);

        glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->m_ProgramHandle, 512, NULL, infoLog);
            std::string errorMessage = "Error: Shader Program - " + std::string(infoLog);
            NX_ERROR(errorMessage);
        }

        glDetachShader(m_ProgramHandle, vertexShader);
        glDetachShader(m_ProgramHandle, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
}