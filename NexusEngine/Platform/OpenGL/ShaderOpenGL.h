#pragma once

#include "GL.h"
#include "Core/Graphics/Shader.h"
#include "Core/nxpch.h"

#include <iostream>

#include <filesystem>

namespace Nexus
{
    struct ShaderSources
    {
        std::string VertexSource;
        std::string FragmentSource;
    };

    static GLenum GetGLBaseType(const BufferElement element)
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

    class ShaderOpenGL : public Shader
    {
        public:
            ShaderOpenGL(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
            {
                Compile(vertexShaderSource, fragmentShaderSource);
                m_Layout = layout;
            }

            ShaderOpenGL(const std::string& filepath, const BufferLayout& layout)
            {
                auto source = ParseShader(filepath);
                Compile(source.VertexSource, source.FragmentSource);
                m_Layout = layout;
            }

            virtual void Bind() override 
            {
                glUseProgram(this->m_ProgramHandle);
                SetLayout();
            }

            virtual void SetShaderUniform1i(const std::string& name, int value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniform1f(loc, value);
            }

            virtual void SetShaderUniform1f(const std::string& name, float value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniform1f(loc, value);
            }

            virtual void SetShaderUniform2f(const std::string& name, const glm::vec2& value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniform2f(loc, value.x, value.y);
            }

            virtual void SetShaderUniform3f(const std::string& name, const glm::vec3& value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniform3f(loc, value.x, value.y, value.z);
            }

            virtual void SetShaderUniform4f(const std::string& name, const glm::vec4& value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniform4f(loc, value.x, value.y, value.z, value.w);
            }

            virtual void SetShaderUniformMat3(const std::string& name, const glm::mat3& value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
            }

            virtual void SetShaderUniformMat4(const std::string& name, const glm::mat4& value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
            }

            virtual const BufferLayout& GetLayout() const override
            { 
                return m_Layout; 
            }

        private:
            ShaderSources ParseShader(const std::string& path)
            {
                std::ifstream stream(path);

                enum class ShaderType
                {
                    None = -1,
                    Vertex = 0,
                    Fragment = 1,
                };

                std::string line;
                std::stringstream ss[2];
                ShaderType type = ShaderType::None;

                while (getline(stream, line))
                {
                    if (line.find("#shader") != std::string::npos)
                    {
                        if (line.find("vertex") != std::string::npos)
                            type = ShaderType::Vertex;
                        else if (line.find("fragment") != std::string::npos)
                            type = ShaderType::Fragment;
                    }
                    else
                    {
                        ss[(int)type] << line << "\n";
                    }
                }

                ShaderSources source;
                source.VertexSource = ss[0].str();
                source.FragmentSource = ss[1].str();
                return source;
            }

            void Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
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
                    std::cout << "ERROR: VERTEX SHADER: " << infoLog << std::endl;
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
                    std::cout << "ERROR: FRAGMENT SHADER: " << infoLog << std::endl;
                }

                this->m_ProgramHandle = glCreateProgram();
                glAttachShader(this->m_ProgramHandle, vertexShader);
                glAttachShader(this->m_ProgramHandle, fragmentShader);
                glLinkProgram(this->m_ProgramHandle);

                glGetProgramiv(this->m_ProgramHandle, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(this->m_ProgramHandle, 512, NULL, infoLog);
                    std::cout << "ERROR: SHADER PROGRAM " << infoLog << std::endl;
                }

                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
            }

            void SetLayout()
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

        private:
            unsigned int m_ProgramHandle;
            BufferLayout m_Layout;
    };
}