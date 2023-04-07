#pragma once

#include <fstream>
#include <string>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Core/Graphics/ShaderDataType.h"

namespace Nexus
{
    uint32_t GetShaderDataTypeSize(ShaderDataType type);

    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        size_t Offset;
        bool Normalized;

        BufferElement() = default;
        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false);
        uint32_t GetComponentCount() const;
    };

    struct BufferLayout
    {
        public:
            BufferLayout(){}

            BufferLayout(std::initializer_list<BufferElement> elements)
                : m_Elements(elements)
            {
                CalculateOffsetsAndStride();
            }

            std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
            std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
            std::vector<BufferElement>::const_iterator begin() const {return m_Elements.begin(); }
            std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

            uint32_t GetStride() { return m_Stride; }
            uint32_t GetNumberOfElements() { return m_Elements.size(); }

        private:
            void CalculateOffsetsAndStride();
        private:
            std::vector<BufferElement> m_Elements;
            uint32_t m_Stride = 0;
    };

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

            virtual const BufferLayout& GetLayout() const = 0;
    };
}