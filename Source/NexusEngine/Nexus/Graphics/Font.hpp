#pragma once

#include "GraphicsDevice.hpp"
#include "Texture.hpp"

#include "Nexus/Point.hpp"
#include "Nexus/Graphics/Color.hpp"

#include <string>

namespace Nexus::Graphics
{
    struct CharacterRange
    {
        uint32_t Begin;
        uint32_t End;
    };

    struct CharacterTextureCoords
    {
        Nexus::Point<float> Min;
        Nexus::Point<float> Max;
    };

    struct FontData
    {
    public:
        FontData() = delete;
        FontData(uint32_t width, uint32_t height)
        {
            m_Width = width;
            m_Height = height;
            m_Pixels.resize(m_Width * m_Height);
        }

        std::vector<uint32_t> &GetPixels()
        {
            return m_Pixels;
        }

        void Clear(unsigned char value)
        {
            for (int i = 0; i < m_Pixels.size(); i++)
            {
                m_Pixels[i] = value;
            }
        }

        void SetPixel(uint32_t x, uint32_t y, unsigned char value)
        {
            x %= m_Width;
            y %= m_Height;

            y = m_Height - y - 1;

            uint32_t offset = x + (y * m_Width);

            Color color = Nexus::Graphics::Color(255, 255, 255, 255);
            m_Pixels[offset] = color.GetColor();

            m_Pixels[offset] = value;
        }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        std::vector<uint32_t> m_Pixels;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };

    struct Character
    {
        glm::vec2 Size;
        glm::vec2 Bearing;
        glm::vec2 TexCoordsMin;
        glm::vec2 TexCoordsMax;
        glm::vec2 Advance;
    };

    class Font
    {
    public:
        Font(const std::string &filepath, uint32_t size, const std::vector<CharacterRange> &characterRanges, GraphicsDevice *device);
        Nexus::Ref<Nexus::Graphics::Texture> GetTexture();
        const Character &GetCharacter(char character);
        uint32_t GetSize() const;
        Nexus::Point<uint32_t> MeasureString(const std::string &text, uint32_t size);
        const glm::vec2 &GetLargestCharacterSize();
        const uint32_t GetLineHeight() const;

    private:
        Nexus::Ref<Nexus::Graphics::Texture> m_Texture = nullptr;
        std::vector<CharacterRange> m_CharacterRanges;
        std::map<char, Character> m_Characters;
        uint32_t m_TextureWidth = 0;
        uint32_t m_TextureHeight = 0;
        uint32_t m_Size = 96;
        glm::vec2 m_LargestCharacterSize;
        uint32_t m_LineHeight = 0;
    };
}