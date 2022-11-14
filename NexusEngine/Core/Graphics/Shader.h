#pragma once

namespace NexusEngine
{
    class Shader
    {
        public:
            Shader(const char* vertexShaderSource, const char* fragmentShaderSource){}
            virtual void Bind() = 0;
    };
}