#pragma once

#include <vector>
#include <initializer_list>
#include <array>

#include "Nexus/Graphics/TextureFormat.hpp"
#include "Nexus/Graphics/DepthFormat.hpp"
#include "Nexus/Graphics/RenderPass.hpp"

#include "Nexus/Memory.hpp"

#define NX_SWAPCHAIN nullptr

namespace Nexus::Graphics
{
    /// @brief A pure virtual class representing an API specific framebuffer
    class Framebuffer
    {
    public:
        /// @brief A constructor that sets the initial specification of a framebuffer
        /// @param renderPass A reference counted pointer to a RenderPass to use when rendering to the framebuffer
        Framebuffer(RenderPass *renderPass)
            : m_Specification(renderPass->GetData<FramebufferSpecification>()) {}

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
        virtual bool HasDepthTexture() { return m_Specification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None; }

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