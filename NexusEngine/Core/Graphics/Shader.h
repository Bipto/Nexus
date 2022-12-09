#pragma once

#include <string>
#include "glm.hpp"

namespace Nexus
{
    class Shader
    {
        public:
            Shader(const char* vertexShaderSource, const char* fragmentShaderSource){}
            virtual void Bind() = 0;
            virtual void SetShaderUniform1i(const std::string& name, int value) = 0;
            virtual void SetShaderUniform4f(const std::string& name, const glm::vec4& value) = 0;
    };
}