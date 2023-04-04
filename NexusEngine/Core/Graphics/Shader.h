#pragma once

#include <fstream>
#include <string>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Core/Graphics/ShaderDataType.h"

namespace Nexus
{
    static uint32_t GetShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:     return 4;
            case ShaderDataType::Float2:    return 4 * 2;
            case ShaderDataType::Float3:    return 4 * 3;
            case ShaderDataType::Float4:    return 4 * 4;
            case ShaderDataType::Int:       return 4;
            case ShaderDataType::Int2:      return 4 * 2;
            case ShaderDataType::Int3:      return 4 * 3;
            case ShaderDataType::Int4:      return 4 * 4;
        }
    }

    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        size_t Offset;
        bool Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) 
            : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
            
        }

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
                case ShaderDataType::Float:     return 1;
                case ShaderDataType::Float2:    return 2;
                case ShaderDataType::Float3:    return 3;
                case ShaderDataType::Float4:    return 4;
                case ShaderDataType::Int:       return 1;
                case ShaderDataType::Int2:      return 2;
                case ShaderDataType::Int3:      return 3;
                case ShaderDataType::Int4:      return 4;
            }
        }
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
            void CalculateOffsetsAndStride()
            {
                size_t offset = 0;
                m_Stride = 0;
                for (auto& element : m_Elements)
                {
                    element.Offset = offset;
                    offset += element.Size;
                    m_Stride += element.Size;
                }
            }
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