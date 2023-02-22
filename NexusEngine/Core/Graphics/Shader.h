#pragma once

#include <fstream>
#include <string>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Nexus
{
    class Shader
    {
        public:
            virtual void Bind() = 0;

            virtual void SetShaderUniform1i(const std::string& name, int value) = 0;

            virtual void SetShaderUniform1f(const std::string& name, float value) = 0;
            virtual void SetShaderUniform2f(const std::string& name, const glm::vec2& value) = 0;
            virtual void SetShaderUniform3f(const std::string& name, const glm::vec3& value) = 0;
            virtual void SetShaderUniform4f(const std::string& name, const glm::vec4& value) = 0;

            virtual void SetShaderUniformMat3(const std::string& name, const glm::mat3& value) = 0;
            virtual void SetShaderUniformMat4(const std::string& name, const glm::mat4& value) = 0;
    };
}