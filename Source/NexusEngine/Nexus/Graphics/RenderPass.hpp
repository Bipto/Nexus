#pragma once

#include "Nexus/Memory.hpp"
#include "Swapchain.hpp"

#include "TextureFormat.hpp"
#include "DepthFormat.hpp"

#include <variant>

namespace Nexus::Graphics
{
    // forward declaration
    class Framebuffer;

    enum class LoadOperation
    {
        Clear,
        Load
    };

    enum class RenderPassDataType
    {
        Framebuffer,
        Swapchain
    };

    struct RenderPassSpecification
    {
        LoadOperation ColorLoadOperation = LoadOperation::Clear;
        LoadOperation StencilDepthLoadOperation = LoadOperation::Clear;
    };

    /// @brief A struct representing the settings to use when creating a framebuffer
    struct FramebufferTextureSpecification
    {
        /// @brief A default constructor initialising to default values
        FramebufferTextureSpecification() = default;

        /// @brief A constructor taking in a texture format to use to create a colour attachment
        /// @param format A texture
        FramebufferTextureSpecification(TextureFormat format)
            : TextureFormat(format) {}

        /// @brief The format to use for a colour attachment
        TextureFormat TextureFormat;
    };

    /// @brief A struct representing a set of colour attachments for a framebuffer
    struct FramebufferColorAttachmentSpecification
    {
        /// @brief A default constructor creating an empty set of colour attachments
        FramebufferColorAttachmentSpecification() = default;

        /// @brief A constructor taking in an initializer list of texture specifications
        /// @param attachments An initializer list of the colour attachments to create
        FramebufferColorAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments(attachments) {}

        /// @brief A vector containing the colour attachments
        std::vector<FramebufferTextureSpecification> Attachments;
    };

    /// @brief A struct representing a depth attachment of a framebuffer
    struct FramebufferDepthAttachmentSpecification
    {
        /// @brief A default constructor creating no depth attachment
        FramebufferDepthAttachmentSpecification() = default;

        /// @brief A constructor taking in a depth format
        /// @param format The depth format to create a depth attachment with
        FramebufferDepthAttachmentSpecification(DepthFormat format)
            : DepthFormat(format) {}

        /// @brief The depth attachment to use to create the depth attachment
        DepthFormat DepthFormat = DepthFormat::None;
    };

    /// @brief A struct representing a framebuffer configuration
    struct FramebufferSpecification
    {
        /// @brief The width of the textures in the framebuffer
        int Width = 1280;

        /// @brief The height of the textures in the framebuffer
        int Height = 720;

        /// @brief Settings to use when creating a set of colour attachments
        FramebufferColorAttachmentSpecification ColorAttachmentSpecification;

        /// @brief Settings to use when creating a depth attachment
        FramebufferDepthAttachmentSpecification DepthAttachmentSpecification;

        /// @brief A boolean value indicating whether the framebuffer is part of the swapchain
        bool IsSwapchain = false;
    };

    typedef std::variant<FramebufferSpecification, Swapchain *> RenderPassData;

    class RenderPass
    {
    public:
        virtual ~RenderPass() {}
        virtual LoadOperation GetColorLoadOperation() = 0;
        virtual LoadOperation GetDepthStencilLoadOperation() = 0;
        virtual const RenderPassSpecification &GetRenderPassSpecification() = 0;
        virtual const RenderPassData &GetRenderPassData() = 0;
        virtual RenderPassDataType GetRenderPassDataType() = 0;

        template <typename T>
        T GetData()
        {
            const auto &initialData = this->GetRenderPassData();
            T data = std::get<T>(initialData);
            return data;
        }
    };
}