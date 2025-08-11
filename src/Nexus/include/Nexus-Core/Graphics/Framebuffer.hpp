#pragma once

#include "Nexus-Core/Graphics/Texture.hpp"
#include "Nexus-Core/Types.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "PixelFormat.hpp"

namespace Nexus::Graphics
{
	/// @brief A struct representing the settings to use when creating a framebuffer
	struct FramebufferTextureSpecification
	{
		/// @brief A default constructor initialising to default values
		FramebufferTextureSpecification() = default;

		/// @brief A constructor taking in a texture format to use to create a colour
		/// attachment
		/// @param format A texture
		FramebufferTextureSpecification(PixelFormat format) : TextureFormat(format)
		{
		}

		/// @brief The format to use for a colour attachment
		PixelFormat TextureFormat;
	};

	/// @brief A struct representing a set of colour attachments for a framebuffer
	struct FramebufferColorAttachmentSpecification
	{
		/// @brief A default constructor creating an empty set of colour attachments
		FramebufferColorAttachmentSpecification() = default;

		/// @brief A constructor taking in an initializer list of texture
		/// specifications
		/// @param attachments An initializer list of the colour attachments to create
		FramebufferColorAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) : Attachments(attachments)
		{
		}

		/// @brief A vector containing the colour attachments
		std::vector<FramebufferTextureSpecification> Attachments = {};
	};

	/// @brief A struct representing a depth attachment of a framebuffer
	struct FramebufferDepthAttachmentSpecification
	{
		/// @brief A default constructor creating no depth attachment
		FramebufferDepthAttachmentSpecification() = default;

		/// @brief A constructor taking in a depth format
		/// @param format The depth format to create a depth attachment with
		FramebufferDepthAttachmentSpecification(PixelFormat format) : DepthFormat(format)
		{
		}

		/// @brief The depth attachment to use to create the depth attachment
		PixelFormat DepthFormat = PixelFormat::Invalid;
	};

	/// @brief A struct representing a framebuffer configuration
	struct FramebufferSpecification
	{
		/// @brief The width of the textures in the framebuffer
		uint32_t Width = 1280;

		/// @brief The height of the textures in the framebuffer
		uint32_t Height = 720;

		/// @brief Settings to use when creating a set of colour attachments
		FramebufferColorAttachmentSpecification ColourAttachmentSpecification;

		/// @brief Settings to use when creating a depth attachment
		FramebufferDepthAttachmentSpecification DepthAttachmentSpecification;

		uint32_t Samples = 1;
	};

	/// @brief A pure virtual class representing an API specific framebuffer
	class Framebuffer
	{
	  public:
		/// @brief A constructor that sets the initial specification of a framebuffer
		/// @param spec A reference to a specification to create the framebuffer with
		Framebuffer(const FramebufferSpecification &spec) : m_Description(spec)
		{
		}

		/// @brief A virtual destructor enabling resources to be cleaned up
		virtual ~Framebuffer() {};

		/// @brief A method to get the number of colour attachments in the framebuffer
		/// @return An integer representing the number of colour attachments
		int GetColorTextureCount()
		{
			return m_Description.ColourAttachmentSpecification.Attachments.size();
		}

		/// @brief A method to check whether a framebuffer has a colour attachment
		/// @return A boolean representing whether a framebuffer has a colour
		/// attachment
		virtual bool HasColorTexture()
		{
			return m_Description.ColourAttachmentSpecification.Attachments.size() > 0;
		}

		/// @brief A method to check whether a framebuffer has a depth attachment
		/// @return A boolean representing whether a framebuffer has a depth
		/// attachment
		virtual bool HasDepthTexture()
		{
			return m_Description.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid;
		}

		/// @brief A pure virtual method to return the FramebufferSpecification
		/// @return The FramebufferSpecification
		virtual const FramebufferSpecification GetFramebufferSpecification() = 0;

		/// @brief A pure virtual method to set the framebuffer specification,
		/// automatically invoking the Recreate() method
		/// @param spec The new framebuffer specification
		virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) = 0;

		/// @brief A pure virtual method to retrieve a color texture from the
		/// framebuffer at the specified index
		/// @param index The index of the texture to retrieve
		/// @return A pointer to a texture object
		virtual Ref<Texture> GetColorTexture(uint32_t index = 0) = 0;

		/// @brief A pure virtual method to retrieve the depth texture from the
		/// framebuffer
		/// @return A pointer to a texture object
		virtual Ref<Texture> GetDepthTexture() = 0;

		void Resize(uint32_t width, uint32_t height)
		{
			auto framebufferSpec   = GetFramebufferSpecification();
			framebufferSpec.Width  = width;
			framebufferSpec.Height = height;
			SetFramebufferSpecification(framebufferSpec);
		}

	  protected:
		/// @brief An object containing the specification of a framebuffer
		FramebufferSpecification m_Description;

	  private:
		/// @brief Recreates the framebuffer to the size specified in the
		/// specification
		virtual void Recreate() = 0;
	};
}	 // namespace Nexus::Graphics