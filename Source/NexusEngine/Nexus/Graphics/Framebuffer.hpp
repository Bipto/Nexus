#pragma once

#include <vector>
#include <initializer_list>
#include <array>

#include "Nexus/Graphics/Multisample.hpp"
#include "Nexus/Types.hpp"
#include "PixelFormat.hpp"

typedef void *FramebufferTexture;

namespace Nexus::Graphics
{
    /// @brief A struct representing the settings to use when creating a framebuffer
    struct FramebufferTextureSpecification
    {
        /// @brief A default constructor initialising to default values
        FramebufferTextureSpecification() = default;

        /// @brief A constructor taking in a texture format to use to create a colour attachment
        /// @param format A texture
        FramebufferTextureSpecification(PixelFormat format)
            : TextureFormat(format) {}

        /// @brief The format to use for a colour attachment
        PixelFormat TextureFormat;
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
        FramebufferDepthAttachmentSpecification(PixelFormat format)
            : DepthFormat(format) {}

        /// @brief The depth attachment to use to create the depth attachment
        PixelFormat DepthFormat = PixelFormat::None;
    };

    /// @brief A struct representing a framebuffer configuration
    struct FramebufferSpecification
    {
        /// @brief The width of the textures in the framebuffer
        uint32_t Width = 1280;

        /// @brief The height of the textures in the framebuffer
        uint32_t Height = 720;

        /// @brief Settings to use when creating a set of colour attachments
        FramebufferColorAttachmentSpecification ColorAttachmentSpecification;

        /// @brief Settings to use when creating a depth attachment
        FramebufferDepthAttachmentSpecification DepthAttachmentSpecification;

        MultiSamples Samples = MultiSamples::SampleCount1;
    };

    /// @brief A pure virtual class representing an API specific framebuffer
    class Framebuffer
    {
    public:
        /// @brief A constructor that sets the initial specification of a framebuffer
        /// @param spec A reference to a specification to create the framebuffer with
        Framebuffer(const FramebufferSpecification &spec)
            : m_Specification(spec) {}

        /// @brief A virtual destructor enabling resources to be cleaned up
        virtual ~Framebuffer(){};

        /// @brief A method to get the number of colour attachments in the framebuffer
        /// @return An integer representing the number of colour attachments
        int GetColorTextureCount() { return m_Specification.ColorAttachmentSpecification.Attachments.size(); }

        /// @brief A method to check whether a framebuffer has a colour attachment
        /// @return A boolean representing whether a framebuffer has a colour attachment
        virtual bool HasColorTexture() { return m_Specification.ColorAttachmentSpecification.Attachments.size() > 0; }

        /// @brief A method to check whether a framebuffer has a depth attachment
        /// @return A boolean representing whether a framebuffer has a depth attachment
        virtual bool HasDepthTexture() { return m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None; }

        /// @brief A pure virtual method to return a colour attachment in the framebuffer
        /// @param index The index of the colour attachment to return within the framebuffer
        /// @return A void pointer to the colour texture
        virtual void *GetColorAttachment(int index = 0) = 0;

        /// @brief A pure virtual method to return the framebuffers depth attachment
        /// @return A void pointer to the depth texture
        virtual void *GetDepthAttachment() = 0;

        /// @brief A pure virtual method to return the FramebufferSpecification
        /// @return The FramebufferSpecification
        virtual const FramebufferSpecification GetFramebufferSpecification() = 0;

        /// @brief A pure virtual method to set the framebuffer specification, automatically invoking the Recreate() method
        /// @param spec The new framebuffer specification
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) = 0;

    protected:
        /// @brief An object containing the specification of a framebuffer
        FramebufferSpecification m_Specification;

    private:
        /// @brief Recreates the framebuffer to the size specified in the specification
        virtual void Recreate() = 0;
    };
}