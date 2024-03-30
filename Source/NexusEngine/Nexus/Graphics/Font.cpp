#include "Font.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

void LoadCharacter(char character, FT_Face &face, Nexus::Graphics::FontData &data, std::map<char, Nexus::Graphics::Character> &characters, uint32_t xPos, uint32_t yPos, uint32_t textureWidth, uint32_t textureHeight)
{
    if (FT_Load_Char(face, character, FT_LOAD_DEFAULT))
    {
        std::cout << "Failed to load glyph: " << character << std::endl;
    }

    FT_GlyphSlot slot = face->glyph;
    FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

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
    c.Advance = {face->glyph->advance.x, face->glyph->advance.y};
    characters[character] = c;
}

Nexus::Point<uint32_t> FindLargestGlyphSize(const FT_Face &face, const std::vector<Nexus::Graphics::CharacterRange> &ranges, uint32_t &numberOfCharacters)
{
    uint32_t width = 0;
    uint32_t height = 0;
    numberOfCharacters = 0;

    for (const auto &range : ranges)
    {
        for (uint32_t i = range.Begin; i < range.End; i++)
        {
            FT_Load_Char(face, (char)i, FT_LOAD_DEFAULT);
            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF);
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
    Font::Font(const std::string &filepath, uint32_t size, const std::vector<CharacterRange> &characterRanges, GraphicsDevice *device)
        : m_CharacterRanges(characterRanges), m_Size(size)
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
        FT_Set_Pixel_Sizes(face, 0, m_Size);

        uint32_t characterCount;
        auto largestCharacterSize = FindLargestGlyphSize(face, m_CharacterRanges, characterCount);

        m_LargestCharacterSize = {largestCharacterSize.X, largestCharacterSize.Y};

        uint32_t columnCount = ceil(sqrt(characterCount));

        m_TextureWidth = columnCount * largestCharacterSize.X;
        m_TextureHeight = columnCount * largestCharacterSize.Y;

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.NumberOfChannels = 4;
        textureSpec.Format = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
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
                LoadCharacter((char)character, face, pixels, m_Characters, xPos, yPos, m_TextureWidth, m_TextureHeight);

                xPos += largestCharacterSize.X;

                if (xPos >= m_TextureWidth)
                {
                    xPos = 0;
                    yPos += largestCharacterSize.Y;
                }
            }
        }

        m_SpaceWidth = GetCharacter('i').Advance.x / 64;

        m_Texture = device->CreateTexture(textureSpec);
        m_Texture->SetData(pixels.GetPixels().data(), pixels.GetPixels().size() * sizeof(uint32_t));

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
        return m_Size;
    }

    Nexus::Point<uint32_t> Font::MeasureString(const std::string &text, uint32_t size)
    {
        float scale = 1.0f / GetSize() * size;
        float xPos = 0.0f;
        float yPos = 0.0f;
        float width = 0.0f;
        float height = GetLargestCharacterSize().y * scale;

        for (auto character : text)
        {
            const auto &characterInfo = GetCharacter(character);

            if (character == ' ')
            {
                xPos += GetSpaceWidth() * scale;
            }
            else if (character == '\t')
            {
                xPos += GetSpaceWidth() * scale * 4;
            }
            else if (character == '\n')
            {
                xPos = 0;
                yPos += height;
                height += GetLargestCharacterSize().y * scale;
            }
            else
            {
                xPos += characterInfo.Bearing.x * scale;
                width = xPos + characterInfo.Size.x * scale;
                xPos += characterInfo.Advance.x / 64 * scale;
            }
        }

        return {(uint32_t)width, (uint32_t)height};
    }

    float Font::GetSpaceWidth()
    {
        return m_SpaceWidth;
    }

    const glm::vec2 &Font::GetLargestCharacterSize()
    {
        return m_LargestCharacterSize;
    }
}