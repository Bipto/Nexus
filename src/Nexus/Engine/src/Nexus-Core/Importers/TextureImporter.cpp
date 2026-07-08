#include "Nexus-Core/Importers/TextureImporter.hpp"

#include "Platform/FileSystem/File.hpp"

#include <RHI/PixelFormat.hpp>

#include <stb_image.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string.h>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    Nexus::PixelType GetPixelType(const stbi_uc *data, int length)
    {
        bool isHdri = static_cast<bool>(stbi_is_hdr_from_memory(
            reinterpret_cast<const stbi_uc *>(data), static_cast<int>(length)
        ));
        if (isHdri)
        {
            return Nexus::PixelType::PixelFloat;
        }

        bool is16Bit = static_cast<bool>(stbi_is_16_bit_from_memory(
            reinterpret_cast<const stbi_uc *>(data), static_cast<int>(length)
        ));
        if (is16Bit)
        {
            return Nexus::PixelType::Pixel16Bit;
        }

        return Nexus::PixelType::Pixel8Bit;
    }

    std::expected<Nexus::TextureData, std::string> LoadStbiImageDataToBuffer(
        const stbi_uc *data, int length, Nexus::PixelType pixelType,
        std::optional<uint32_t> desiredChannels
    )
    {
        int width = 0, height = 0, channels = 0;
        int targetChannels =
            desiredChannels.has_value() ? static_cast<int>(*desiredChannels) : 0;

        switch (pixelType)
        {
        case Nexus::PixelType::PixelFloat:
        {
            std::unique_ptr<float, void (*)(void *)> imageData(
                stbi_loadf_from_memory(
                    data, length, &width, &height, &channels, targetChannels
                ),
                stbi_image_free
            );

            if (!imageData)
            {
                return std::unexpected("Failed to load image data");
            }

            int actualChannels =
                desiredChannels.has_value() ? *desiredChannels : channels;

            std::vector<std::byte> pixelData(
                static_cast<size_t>(width * height * actualChannels * sizeof(float))
            );
            memcpy(pixelData.data(), imageData.get(), pixelData.size());

            return Nexus::TextureData{
                .Info =
                    {
                        .Width = static_cast<uint32_t>(width),
                        .Height = static_cast<uint32_t>(height),
                        .Channels = static_cast<uint32_t>(actualChannels),
                        .Type = pixelType,
                    },
                .Pixels = pixelData,
            };
        }
        case Nexus::PixelType::Pixel16Bit:
        {
            std::unique_ptr<stbi_us, void (*)(void *)> imageData(
                stbi_load_16_from_memory(
                    data, length, &width, &height, &channels, targetChannels
                ),
                stbi_image_free
            );

            if (!imageData)
            {
                return std::unexpected("Failed to load image data");
            }

            int actualChannels =
                desiredChannels.has_value() ? *desiredChannels : channels;

            std::vector<std::byte> pixelData(
                static_cast<size_t>(
                    width * height * actualChannels * sizeof(uint16_t)
                )
            );
            memcpy(pixelData.data(), imageData.get(), pixelData.size());

            return Nexus::TextureData{
                .Info =
                    {
                        .Width = static_cast<uint32_t>(width),
                        .Height = static_cast<uint32_t>(height),
                        .Channels = static_cast<uint32_t>(actualChannels),
                        .Type = pixelType,
                    },
                .Pixels = pixelData,
            };
        }
        case Nexus::PixelType::Pixel8Bit:
        {
            std::unique_ptr<stbi_uc, void (*)(void *)> imageData(
                stbi_load_from_memory(
                    data, length, &width, &height, &channels, targetChannels
                ),
                stbi_image_free
            );

            if (!imageData)
            {
                return std::unexpected("Failed to load image data");
            }

            int actualChannels =
                desiredChannels.has_value() ? *desiredChannels : channels;

            std::vector<std::byte> pixelData(
                static_cast<size_t>(
                    width * height * actualChannels * sizeof(uint8_t)
                )
            );
            memcpy(pixelData.data(), imageData.get(), pixelData.size());

            return Nexus::TextureData{
                .Info =
                    {
                        .Width = static_cast<uint32_t>(width),
                        .Height = static_cast<uint32_t>(height),
                        .Channels = static_cast<uint32_t>(actualChannels),
                        .Type = pixelType,
                    },
                .Pixels = pixelData,
            };
        }
        default:
            return std::unexpected("Failed to find a valid pixel type");
        }
    }
} // namespace

namespace Nexus
{
    std::expected<TextureInfo, std::string> TextureImporter::GetTextureInfoFromDisk(
        IResourceLoader *loader, std::string_view path
    ) const
    {
        std::string filepath = std::string(path);
        return loader->LoadBytes(path).and_then(
            [this](std::vector<std::byte> pixels) {
                return GetTextureInfoFromMemory(pixels);
            }
        );
    }

    std::expected<TextureInfo, std::string> TextureImporter::
        GetTextureInfoFromMemory(std::span<std::byte> buffer) const
    {
        int width = 0, height = 0, channels = 0;
        int result = stbi_info_from_memory(
            reinterpret_cast<const stbi_uc *>(buffer.data()),
            static_cast<int>(buffer.size()), &width, &height, &channels
        );

        if (result)
        {
            PixelType pixelType = GetPixelType(
                reinterpret_cast<const stbi_uc *>(buffer.data()),
                static_cast<int>(buffer.size())
            );
            return TextureInfo{
                .Width = static_cast<uint32_t>(width),
                .Height = static_cast<uint32_t>(height),
                .Channels = static_cast<uint32_t>(channels),
                .Type = pixelType,
            };
        }

        return std::unexpected("The image format is unsupported");
    }

    std::expected<TextureData, std::string> TextureImporter::LoadImageFromDisk(
        IResourceLoader *loader, std::string_view path, bool flipVertically,
        std::optional<uint32_t> desiredChannels
    ) const
    {
        std::string filepath = std::string(path);
        return loader->LoadBytes(path).and_then(
            [this, flipVertically, desiredChannels](std::vector<std::byte> pixels) {
                return LoadImageFromMemory(pixels, flipVertically, desiredChannels);
            }
        );
    }

    std::expected<TextureData, std::string> TextureImporter::LoadImageFromMemory(
        std::span<std::byte> buffer, bool flipVertically,
        std::optional<uint32_t> desiredChannels
    ) const
    {
        stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));

        PixelType pixelType = GetPixelType(
            reinterpret_cast<const stbi_uc *>(buffer.data()),
            static_cast<int>(buffer.size())
        );

        return LoadStbiImageDataToBuffer(
            reinterpret_cast<const stbi_uc *>(buffer.data()),
            static_cast<int>(buffer.size()), pixelType, desiredChannels
        );
    }
} // namespace Nexus