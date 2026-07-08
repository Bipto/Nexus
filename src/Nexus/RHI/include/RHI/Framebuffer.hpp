#pragma once

#include <cstdint>
#include <optional>
#include <utility>

#include "Core/ResourcePool.hpp"

#include "RHI/PixelFormat.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
    /// @brief A structure describing a section of a texture to be bound as a target
    /// for a framebuffer
    struct FramebufferTextureDescription
    {
        /// @brief The start array layer within the texture to bind
        uint32_t BaseArrayLayer = 0;

        /// @brief The number of array layers to bind (used for multiview rendering)
        uint32_t LayerCount = 0;

        /// @brief The mip level within the texture to bind
        uint32_t MipLevel = 0;

        /// @brief A handle to the texture to bind
        TextureHandle TargetTexture = {};
    };

    struct FramebufferColourAttachmentDescription
    {
        FramebufferTextureDescription ColourAttachment = {};
        std::optional<FramebufferTextureDescription> ResolveAttachment = {};
    };

    /// @brief A structure describing how to create a framebuffer from an existing
    /// set of textures
    struct FramebufferTextureSetDescription
    {
        /// @brief The colour attachments to use in the framebuffer
        std::vector<FramebufferColourAttachmentDescription> ColourAttachments = {};

        /// @brief The optional depth attachment to use in the framebuffer
        std::optional<FramebufferTextureDescription> DepthAttachment = {};

        /// @brief Utility function to check that all textures have a matching sample
        /// count
        /// @return A boolean value indicating whether the textures have matching
        /// sample counts
        bool ValidateSamples() const;

        /// @brief Utility function to check that all textures have matching
        /// dimensions
        /// @return A boolean value indicating whether the textures having matching
        /// dimensions
        bool ValidateDimensions() const;

        /// @brief Utility function to check that the framebuffer has a valid texture
        /// attached to it
        /// @return A boolean value indicating whether there is at least one valid
        /// texture
        bool ValidateHasTexture() const;

        /// @brief Utility function to check that all textures in the framebuffer
        /// have the correct flags to allow them to be used
        /// @return A boolean value indicating whether the flags are incorrect
        bool ValidateUsageFlags() const;

        /// @brief Utility function to retrieve the sample count that is used in the
        /// framebuffer
        /// @return The sample count of the framebuffer
        uint32_t GetSampleCount() const;

        /// @brief Utility function to retrieve the size of a framebuffer
        /// @return The dimensions of the framebuffer
        std::pair<uint32_t, uint32_t> GetSize() const;

        /// @brief A boolean value indicating whether the textures are associated
        /// with a swapchain
        bool OwnedBySwapchain = false;
    };

    /// @brief A struct representing a framebuffer configuration using new textures
    struct FramebufferTextureCreateDescription
    {
        /// @brief The width of the textures in the framebuffer
        uint32_t Width = 1280;

        /// @brief The height of the textures in the framebuffer
        uint32_t Height = 720;

        /// @brief The pixel formats to use when creating a set of colour attachments
        std::vector<PixelFormat> ColourAttachmentFormats = {};

        /// @brief Settings to use when creating a depth attachment
        std::optional<PixelFormat> DepthAttachmentFormat = {};

        /// @brief The number of samples to use in the framebuffer
        uint32_t Samples = 1;
    };

    /// @brief A pure virtual class representing an API specific framebuffer
    class IFramebuffer
    {
      public:
        /// @brief A virtual destructor enabling resources to be cleaned up
        virtual ~IFramebuffer() {};

        /// @brief A pure virtual method to return the FramebufferSpecification
        /// @return The FramebufferSpecification
        virtual const FramebufferTextureSetDescription
        GetTextureSetDescription() const = 0;

        /// @brief A pure virtual method to retrieve a color texture from the
        /// framebuffer at the specified index
        /// @param index The index of the texture to retrieve
        /// @return A struct containing how the texture is bound to the framebuffer
        std::optional<FramebufferColourAttachmentDescription> GetColorTextureBinding(
            uint32_t index = 0
        )
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            if (index < desc.ColourAttachments.size())
            {
                return desc.ColourAttachments.at(index);
            }
            else
            {
                return {};
            }
        }

        /// @brief A method to retrieve a color texture from the
        /// framebuffer at the specified index
        /// @param index The index of the texture to retrieve
        /// @return A pointer to a texture object
        TextureHandle GetColorTextureHandle(uint32_t index = 0)
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            if (index < desc.ColourAttachments.size())
            {
                return desc.ColourAttachments.at(index)
                    .ColourAttachment.TargetTexture;
            }
            else
            {
                return {};
            }
        }

        /// @brief A method to retrieve a color texture from the
        /// framebuffer at the specified index
        /// @param index The index of the texture to retrieve
        /// @return A pointer to a texture object
        TextureHandle GetResolveTextureHandle(uint32_t index = 0)
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            if (index < desc.ColourAttachments.size())
            {
                std::optional<FramebufferTextureDescription> resolveDesc =
                    desc.ColourAttachments.at(index).ResolveAttachment;
                if (resolveDesc.has_value())
                {
                    return resolveDesc.value().TargetTexture;
                }
                else
                {
                    return {};
                }
            }
            else
            {
                return {};
            }
        }

        /// @brief A method to retrieve the depth texture from the
        /// framebuffer
        /// @return A struct describing how the texture is bound to the framebuffer
        std::optional<FramebufferTextureDescription> GetDepthTextureBinding()
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            if (desc.DepthAttachment.has_value())
            {
                return desc.DepthAttachment.value();
            }
            else
            {
                return {};
            }
        }

        /// @brief A method to retrieve the depth texture from the
        /// framebuffer
        /// @return A pointer to the texture
        TextureHandle GetDepthTextureHandle()
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            if (desc.DepthAttachment.has_value())
            {
                return desc.DepthAttachment.value().TargetTexture;
            }
            else
            {
                return {};
            }
        }

        /// @brief A method to get the number of colour attachments in the
        /// framebuffer
        /// @return An integer representing the number of colour attachments
        size_t GetColorTextureCount()
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            return desc.ColourAttachments.size();
        }

        /// @brief A method to check whether a framebuffer has a colour attachment
        /// @return A boolean representing whether a framebuffer has a colour
        /// attachment
        virtual bool HasColorTexture()
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            return desc.ColourAttachments.size() > 0;
        }

        /// @brief A method to check whether a framebuffer has a depth attachment
        /// @return A boolean representing whether a framebuffer has a depth
        /// attachment
        virtual bool HasDepthTexture()
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            return desc.DepthAttachment.has_value();
        }

        /// @brief A method to get the size of the framebuffer
        /// @return A pair of two 32 bit unsigned integers containing the size of the
        /// framebuffer
        std::pair<uint32_t, uint32_t> GetSize() const
        {
            const FramebufferTextureSetDescription &desc =
                GetTextureSetDescription();
            return desc.GetSize();
        }

        /// @brief A method to get the width of a framebuffer
        /// @return A 32 bit unsigned integer containing the width of the framebuffer
        uint32_t GetWidth() const
        {
            auto [width, height] = GetSize();
            return width;
        }

        /// @brief A method to get the height of a framebuffer
        /// @return A 32 bit unsigned integer containing the height of the
        /// framebuffer
        uint32_t GetHeight() const
        {
            auto [width, height] = GetSize();
            return height;
        }

        /// @brief A method to retrieve the number of samples used by the framebuffer
        /// @return A 32 but unsigned integer indicating the sample count
        uint32_t GetSampleCount() const
        {
            const auto &desc = GetTextureSetDescription();
            return desc.GetSampleCount();
        }

        /// @brief A method to check whether the framebuffer is owned by a swapchain
        /// @return A boolean indicating whether the framebuffer is owned by a
        /// swapchain
        bool IsOwnedBySwapchain() const
        {
            const auto &desc = GetTextureSetDescription();
            return desc.OwnedBySwapchain;
        }
    };

    DEFINE_RESOURCE(Framebuffer, IFramebuffer);
} // namespace Nexus::Graphics