#include "Font.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

void LoadCharacter(char character, FT_Face &face, Nexus::Graphics::FontData &data, std::map<char, Nexus::Graphics::Character> &characters, uint32_t xPos, uint32_t yPos, uint32_t textureWidth, uint32_t textureHeight, FT_Render_Mode renderMode)
{
    if (FT_Load_Char(face, character, FT_LOAD_DEFAULT))
    {
        std::cout << "Failed to load glyph: " << character << std::endl;
    }

    FT_GlyphSlot slot = face->glyph;
    FT_Render_Glyph(slot, renderMode);

    for (int y = 0; y < face->glyph->bitmap.rows; y++)
    {
        for (int x = 0; x < face->glyph->bitmap.width; x++)
        {
            unsigned char pixel = face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.width)];
            data.SetPixel(x + xPos, y + yPos, pixel);
        }
    }

    float u = 1 / (float)textureWidth * (float)xPos;
    float v = 1 / (float)textureHeight * (float)yPos;

    glm::vec2 texCoordsMin;
    texCoordsMin.x = u;
    texCoordsMin.y = 1.0f - v - (1 / (float)textureHeight * face->glyph->bitmap.rows);

    glm::vec2 texCoordsMax;
    texCoordsMax.x = u + (1 / (float)textureWidth * face->glyph->bitmap.width);
    texCoordsMax.y = 1.0f - v;

    Nexus::Graphics::Character c;
    c.Size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
    c.Bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
    c.TexCoordsMin = texCoordsMin;
    c.TexCoordsMax = texCoordsMax;
    c.Advance = face->glyph->advance.x / 64;
    characters[character] = c;
}

Nexus::Point<uint32_t> FindLargestGlyphSize(const FT_Face &face, const std::vector<Nexus::Graphics::CharacterRange> &ranges, uint32_t &numberOfCharacters, FT_Render_Mode renderMode)
{
    uint32_t width = 0;
    uint32_t height = 0;
    numberOfCharacters = 0;

    for (const auto &range : ranges)
    {
        for (uint32_t i = range.Begin; i < range.End; i++)
        {
            FT_Load_Char(face, (char)i, FT_LOAD_RENDER);
            FT_Render_Glyph(face->glyph, renderMode);
            if (face->glyph->bitmap.width > width)
            {
                width = face->glyph->bitmap.width;
            }
            if (face->glyph->bitmap.rows > height)
            {
                height = face->glyph->bitmap.rows;
            }

            numberOfCharacters++;
        }
    }

    return Nexus::Point<uint32_t>(width, height);
}

namespace Nexus::Graphics
{
    Font::Font(const std::string &filepath, uint32_t size, const std::vector<CharacterRange> &characterRanges, FontType type, GraphicsDevice *device)
        : m_CharacterRanges(characterRanges), m_FontSize(size), m_Type(type)
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "Could not initialise FreeType" << std::endl;
        }

        FT_Face face;
        if (FT_New_Face(ft, filepath.c_str(), 0, &face))
        {
            std::cout << "Failed to load font" << std::endl;
        }
        FT_Set_Pixel_Sizes(face, 0, m_FontSize);

        m_LineSpacing = face->size->metrics.height / 64;
        m_UnderlinePosition = face->underline_position;

        FT_Render_Mode renderMode = FT_RENDER_MODE_NORMAL;
        if (type == FontType::SDF)
        {
            renderMode = FT_RENDER_MODE_SDF;
        }

        uint32_t characterCount = 0;
        m_MaxCharacterSize = FindLargestGlyphSize(face, characterRanges, characterCount, renderMode);
        uint32_t columnCount = ceil(sqrt(characterCount));

        m_TextureWidth = columnCount * m_MaxCharacterSize.X;
        m_TextureHeight = columnCount * m_MaxCharacterSize.Y;

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.Format = Nexus::Graphics::PixelFormat::R8_UNorm;
        textureSpec.Width = m_TextureWidth;
        textureSpec.Height = m_TextureHeight;

        FontData pixels(textureSpec.Width, textureSpec.Height);
        pixels.Clear(0);

        uint32_t xPos = 0;
        uint32_t yPos = 0;
        for (const auto &range : m_CharacterRanges)
        {
            for (int character = range.Begin; character < range.End; character++)
            {
                LoadCharacter((char)character, face, pixels, m_Characters, xPos, yPos, m_TextureWidth, m_TextureHeight, renderMode);

                xPos += m_MaxCharacterSize.X;

                if (xPos >= m_TextureWidth)
                {
                    xPos = 0;
                    yPos += m_MaxCharacterSize.Y;
                }
            }
        }

        m_Texture = device->CreateTexture(textureSpec);
        m_Texture->SetData(pixels.GetPixels().data(), 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    Nexus::Ref<Nexus::Graphics::Texture> Font::GetTexture()
    {
        return m_Texture;
    }

    const Character &Font::GetCharacter(char character)
    {
        return m_Characters[character];
    }

    uint32_t Font::GetSize() const
    {
        return m_FontSize;
    }

    Nexus::Point<uint32_t> Font::MeasureString(const std::string &text, uint32_t size)
    {
        float scale = 1.0f / GetSize() * size;

        float x = 0;
        float y = 0;

        if (text.length() > 0)
        {
            y = m_LineSpacing * scale;
            y += (m_LineSpacing / 2) * scale;
        }

        for (auto character : text)
        {
            const auto &characterInfo = GetCharacter(character);

            if (character == '\n')
            {
                x = 0;
                y += m_LineSpacing * scale;
            }
            else if (character == '\t')
            {
                const auto &spaceInfo = GetCharacter(character);
                x += spaceInfo.Advance * scale * 4;
            }
            else
            {
                x += characterInfo.Advance * scale;
            }
        }

        return {(uint32_t)x, (uint32_t)y};
    }

    const uint32_t Font::GetLineHeight() const
    {
        return m_LineSpacing;
    }

    const Point<uint32_t> Font::GetMaxCharacterSize() const
    {
        return m_MaxCharacterSize;
    }

    const FontType Font::GetFontType() const
    {
        return m_Type;
    }
}