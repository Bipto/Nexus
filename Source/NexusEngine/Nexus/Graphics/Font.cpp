#include "Font.hpp"

Nexus::Point<uint32_t> GetLargestCharacterSize(const FT_Face &face, const std::vector<Nexus::Graphics::CharacterRange> &ranges, uint32_t &numberOfCharacters)
{
    uint32_t width = 0;
    uint32_t height = 0;
    numberOfCharacters = 0;

    for (const auto &range : ranges)
    {
        for (uint32_t i = range.Begin; i < range.End; i++)
        {
            FT_Load_Char(face, (char)i, FT_LOAD_RENDER);
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
    Font::Font(const std::string &filepath, const std::vector<CharacterRange> &characterRanges, GraphicsDevice *device)
        : m_CharacterRanges(characterRanges)
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
        FT_Set_Pixel_Sizes(face, 0, 64);

        uint32_t characterCount;
        auto size = GetLargestCharacterSize(face, m_CharacterRanges, characterCount);
        uint32_t columnCount = ceil(sqrt(characterCount));

        m_TextureWidth = columnCount * size.X;
        m_TextureHeight = columnCount * size.Y;

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.NumberOfChannels = 4;
        textureSpec.Format = Nexus::Graphics::TextureFormat::RGBA8;
        textureSpec.Width = m_TextureWidth;
        textureSpec.Height = m_TextureHeight;
        textureSpec.SamplerState = SamplerState::LinearWrap;

        FontData pixels(textureSpec.Width, textureSpec.Height);
        pixels.Clear(0);

        uint32_t xPos = 0;
        uint32_t yPos = 0;
        for (const auto &range : m_CharacterRanges)
        {
            for (int character = range.Begin; character < range.End; character++)
            {
                LoadCharacter((char)character, face, pixels, xPos, yPos);

                xPos += size.X;

                if (xPos >= m_TextureWidth)
                {
                    xPos = 0;
                    yPos += size.Y;
                }
            }
        }

        textureSpec.Data = pixels.GetPixels().data();
        m_Texture = device->CreateTexture(textureSpec);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    Nexus::Graphics::Texture *Font::GetTexture()
    {
        return m_Texture;
    }

    void Font::LoadCharacter(char character, FT_Face &face, FontData &data, uint32_t xPos, uint32_t yPos)
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

        float u = 1 / (float)m_TextureWidth * (float)xPos;
        float v = 1 / (float)m_TextureHeight * (float)yPos;

        glm::vec2 texCoordsMin;
        texCoordsMin.x = u;
        texCoordsMin.y = 1.0f - v - (1 / (float)m_TextureHeight * face->glyph->bitmap.rows);

        glm::vec2 texCoordsMax;
        texCoordsMax.x = u + (1 / (float)m_TextureWidth * face->glyph->bitmap.width);
        texCoordsMax.y = 1.0f - v;

        Character c;
        c.Size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
        c.Bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
        c.TexCoordsMin = texCoordsMin;
        c.TexCoordsMax = texCoordsMax;
        c.Advance = {(uint32_t)face->glyph->advance.x, (uint32_t)face->glyph->advance.y};
        m_Characters[character] = c;
    }

    const Character &Font::GetCharacter(char character)
    {
        return m_Characters[character];
    }
}