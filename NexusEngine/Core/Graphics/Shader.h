#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderDataType.h"
#include "Texture.h"
#include "Core/Memory.h"

namespace Nexus
{
    struct VertexBufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size = 0;
        size_t Offset = 0;
        bool Normalized;

        VertexBufferElement() = default;
        VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false);
        uint32_t GetComponentCount() const;
    };

    struct VertexBufferLayout
    {
        public:
            VertexBufferLayout(){}

            VertexBufferLayout(std::initializer_list<VertexBufferElement> elements)
                : m_Elements(elements)
            {
                CalculateOffsetsAndStride();
            }

            std::vector<VertexBufferElement>::iterator begin() { return m_Elements.begin(); }
            std::vector<VertexBufferElement>::iterator end() { return m_Elements.end(); }
            std::vector<VertexBufferElement>::const_iterator begin() const {return m_Elements.begin(); }
            std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }

            uint32_t GetStride() { return m_Stride; }
            uint32_t GetNumberOfElements() { return m_Elements.size(); }

        private:
            void CalculateOffsetsAndStride();
        private:
            std::vector<VertexBufferElement> m_Elements;
            uint32_t m_Stride = 0;
    };

    struct TextureBinding
    {
        int Slot;
        std::string Name;
    };

    class Shader
    {
        public:
            virtual void SetTexture(Ref<Texture> texture, const TextureBinding& binding) = 0;
            virtual const std::string& GetVertexShaderSource() = 0;
            virtual const std::string& GetFragmentShaderSource() = 0;
            virtual const VertexBufferLayout& GetLayout() const = 0;
    };
}