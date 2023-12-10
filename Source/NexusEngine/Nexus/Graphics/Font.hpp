#pragma once

#include "GraphicsDevice.hpp"
#include "Texture.hpp"

#include "Nexus/Point.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <string>

namespace Nexus::Graphics
{
    struct CharacterRange
    {
        uint32_t Begin;
        uint32_t End;
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

        std::vector<unsigned char> &GetPixels()
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
            m_Pixels[offset] = value;
        }

        operator std::vector<unsigned char>() const
        {
            return m_Pixels;
        }

    private:
        std::vector<unsigned char> m_Pixels;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };

    class Font
    {
    public:
        Font(const std::string &filepath, const std::vector<CharacterRange> &characterRanges, GraphicsDevice *device);
        Nexus::Graphics::Texture *GetTexture();

    private:
        void LoadCharacters(FT_Face &face, FontData &data, uint32_t rowColumnCount, uint32_t maxCharacterWidth, uint32_t maxCharacterHeight);
        void LoadCharacter(char character, FT_Face &face, FontData &data, uint32_t x, uint32_t y);

    private:
        Nexus::Graphics::Texture *m_Texture = nullptr;
        std::vector<CharacterRange> m_CharacterRanges;
    };
}