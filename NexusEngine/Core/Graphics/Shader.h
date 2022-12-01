#pragma once

#include <string>

namespace Nexus
{
    class Shader
    {
        public:
            Shader(const char* vertexShaderSource, const char* fragmentShaderSource){}
            virtual void Bind() = 0;
            virtual void SetShaderUniform1i(const std::string& name, int value) = 0;
    };
}