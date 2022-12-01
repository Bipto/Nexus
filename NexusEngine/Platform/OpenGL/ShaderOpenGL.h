#pragma once

#include "GL.h"
#include "../../Core/Graphics/Shader.h"

#include "../../Core/nxpch.h"

namespace NexusEngine
{
    class ShaderOpenGL : public Shader
    {
        public:
            ShaderOpenGL(const char* vertexShaderSource, const char* fragmentShaderSource) : Shader(vertexShaderSource, fragmentShaderSource)
            {
                unsigned int vertexShader;
                vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
                glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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
            ShaderOpenGL(const ShaderOpenGL&) = delete;

            virtual void Bind() override 
            {
                glUseProgram(this->m_ProgramHandle);
            }

            virtual void SetShaderUniform1i(const std::string& name, int value) override
            {
                unsigned int loc = glGetUniformLocation(this->m_ProgramHandle, name.c_str());
                glUniform1f(loc, value);
            }

        private:
            unsigned int m_ProgramHandle;
    };
}