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
        FT_Set_Pixel_Sizes(face, 0, 96);

        uint32_t characterCount;
        auto size = GetLargestCharacterSize(face, m_CharacterRanges, characterCount);
        uint32_t columnCount = ceil(sqrt(characterCount));

        if (FT_Load_Char(face, 'a', FT_LOAD_RENDER))
        {
            std::cout << "Failed to load glyph" << std::endl;
        }

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.NumberOfChannels = 1;
        textureSpec.Format = Nexus::Graphics::TextureFormat::R8;
        textureSpec.Width = columnCount * size.X;
        textureSpec.Height = columnCount * size.Y;

        FontData pixels(textureSpec.Width, textureSpec.Height);
        pixels.Clear(0);
        // LoadCharacters(face, pixels, columnCount, size.X, size.Y);
        // LoadCharacter('a', face, pixels, 100, 100);

        /* for (int y = 0; y < face->glyph->bitmap.rows; y++)
        {
            for (int x = 0; x < face->glyph->bitmap.width; x++)
            {
                unsigned char pixel = face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.width)];
                pixels.SetPixel(x + 100, y + 100, pixel);
            }
        } */

        uint32_t xPos = 0;
        uint32_t yPos = 0;
        for (const auto &range : m_CharacterRanges)
        {
            for (int character = range.Begin; character < range.End; character++)
            {
                LoadCharacter((char)character, face, pixels, xPos, yPos);

                xPos += size.X;

                if (xPos > columnCount * size.X)
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

    void Font::LoadCharacters(FT_Face &face, FontData &data, uint32_t columnCount, uint32_t maxCharacterWidth, uint32_t maxCharacterHeight)
    {
        uint32_t xIndex = 0;
        uint32_t yIndex = 0;

        for (const auto &range : m_CharacterRanges)
        {
            for (int i = range.Begin; i < range.End; i++)
            {
                char character = (char)i;
                if (FT_Load_Char(face, character, FT_LOAD_RENDER))
                {
                    std::cout << "Failed to load glyph: " << character << std::endl;
                }

                /* for (uint32_t yPos = 0; yPos < face->glyph->bitmap.rows; yPos++)
                {
                    for (uint32_t xPos = 0; xPos < face->glyph->bitmap.width; xPos++)
                    {
                        unsigned char pixel = face->glyph->bitmap.buffer[xPos + (yPos * face->glyph->bitmap.width)];
                        data.SetPixel(xPos * maxCharacterWidth, yPos * maxCharacterHeight, pixel);
                    }
                } */

                for (int y = 0; y < face->glyph->bitmap.rows; y++)
                {
                    for (int x = 0; x < face->glyph->bitmap.width; x++)
                    {
                        unsigned char pixel = face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.width)];
                    }
                }

                if (xIndex > columnCount)
                {
                    yIndex = 0;
                    xIndex++;
                }
            }
        }
    }

    void Font::LoadCharacter(char character, FT_Face &face, FontData &data, uint32_t xPos, uint32_t yPos)
    {
        if (FT_Load_Char(face, character, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load glyph: " << character << std::endl;
        }

        for (int y = 0; y < face->glyph->bitmap.rows; y++)
        {
            for (int x = 0; x < face->glyph->bitmap.width; x++)
            {
                unsigned char pixel = face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.width)];
                data.SetPixel(x + xPos, y + yPos, pixel);
            }
        }
    }

    /*void Font::LoadCharacter(char character, const FT_Face &face, FontData &data)
    {
        if (FT_Load_Char(face, character, FT_LOAD_RENDER))
        {
        }
    }

     Point<uint32_t> Font::GetLargestCharacterSize(const FT_Face &face)
    {
        uint32_t width = 0;
        uint32_t height = 0;

        for (const auto &range : m_CharacterRanges)
        {
            for (char i = range.Begin; i < range.End; i++)
            {
                FT_Load_Char(face, (char)i, FT_LOAD_RENDER);
                if (face->glyph->bitmap.width > width)
                {
                    width = face->glyph->bitmap.width;
                }
                if (face->glyph->bitmap.rows > height)
                {
                    height = face->glyph->bitmap.rows;
                }
            }
        }

        return {width, height};
    } */
}