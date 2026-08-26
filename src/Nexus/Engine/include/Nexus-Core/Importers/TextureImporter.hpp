#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/Resources/IResourceLoader.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "RHI/PixelFormat.hpp"

namespace Nexus
{
    enum class PixelType
    {
        Pixel8Bit,
        Pixel16Bit,
        PixelFloat,
    };

    struct TextureInfo
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Channels = 0;
        PixelType Type = PixelType::Pixel8Bit;
    };

    struct TextureData
    {
        TextureInfo Info = {};
        std::vector<std::byte> Pixels = {};
    };

    class NX_API TextureImporter
    {
      public:
        TextureImporter() = default;

        [[nodiscard]] std::expected<TextureInfo, std::string> GetTextureInfoFromDisk(IResourceLoader *loader,
                                                                                     std::string_view path) const;

        [[nodiscard]] std::expected<TextureInfo, std::string> GetTextureInfoFromMemory(
            std::span<std::byte> buffer) const;

        [[nodiscard]] std::expected<TextureData, std::string> LoadImageFromDisk(
            IResourceLoader *loader, std::string_view path, bool flipVertically,
            std::optional<uint32_t> desiredChannels) const;

        [[nodiscard]] std::expected<TextureData, std::string> LoadImageFromMemory(
            std::span<std::byte> buffer, bool flipVertically, std::optional<uint32_t> desiredChannels) const;
    };
} // namespace Nexus