#if defined(NX_PLATFORM_OPENGL)

	#include "TextureOpenGL.hpp"

	#include "GL.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

	#include <stb_image_write.h>

namespace Nexus::Graphics
{
	TextureOpenGL::TextureOpenGL(const TextureSpecification &spec, GraphicsDeviceOpenGL *graphicsDevice) : Texture(spec), m_Device(graphicsDevice)
	{
		NX_ASSERT(spec.ArrayLayers >= 1, "Texture must have at least one array layer");
		NX_ASSERT(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Usage & TextureUsage_Cubemap)
		{
			NX_ASSERT(spec.ArrayLayers % 6 == 0, "Cubemap texture must have 6 array layers");
		}

		if (spec.Samples > 1)
		{
			NX_ASSERT(spec.MipLevels == 0, "Multisampled textures do not support mipmapping");
		}

		bool isDepth			  = spec.Usage & Graphics::TextureUsage_DepthStencil;
		m_TextureType			  = GL::GetTextureType(spec);
		m_InternalFormat		  = GL::GetSizedInternalFormat(spec.Format, isDepth);
		m_BaseType				  = GL::GetPixelType(spec.Format);
		m_GLInternalTextureFormat = GL::GetGLInternalTextureFormat(spec);
		m_DataFormat			  = GL::GetPixelDataFormat(spec.Format);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.EXT_direct_state_access || context.ARB_direct_state_access)
				{
					CreateTextureFacesDSA(context);
				}
				else
				{
					CreateTextureFacesNonDSA(context);
				}
			});
	}

	TextureOpenGL::~TextureOpenGL()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteTextures(1, &m_Handle); });
	}

	void TextureOpenGL::Bind(uint32_t slot)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_direct_state_access || context.EXT_direct_state_access)
				{
					glCall(context.BindTextureUnit(slot, m_Handle));
				}
				else
				{
					glCall(context.ActiveTexture(GL_TEXTURE0 + slot));
					glCall(context.BindTexture(m_TextureType, m_Handle));
				}
			});
	}

	uint32_t TextureOpenGL::GetHandle()
	{
		return m_Handle;
	}

	GLenum TextureOpenGL::GetTextureType()
	{
		return m_TextureType;
	}

	GLenum TextureOpenGL::GetDataFormat()
	{
		return m_DataFormat;
	}

	GLenum TextureOpenGL::GetBaseType()
	{
		return m_BaseType;
	}

	void TextureOpenGL::CreateTextureFacesDSA(const GladGLContext &context)
	{
		context.CreateTextures(m_TextureType, 1, &m_Handle);
		context.TextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		context.TextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		context.PixelStorei(GL_PACK_ALIGNMENT, 1);
		context.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

		switch (m_GLInternalTextureFormat)
		{
			case GL::GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TextureStorage1D(m_Handle, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width));
				break;
	#else
				throw std::runtime_error("1D textures are not supported by WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(
					context.TextureStorage2D(m_Handle, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width, m_Specification.Height));
				break;
			case GL::GLInternalTextureFormat::Texture2DMultisample:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TextureStorage2DMultisample(m_Handle,
														   m_Specification.MipLevels,
														   m_InternalFormat,
														   m_Specification.Width,
														   m_Specification.Height,
														   GL_TRUE));
				break;
	#else
				throw std::runtime_error("Multisampled textures are not supported by WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
				glCall(context.TextureStorage3D(m_Handle,
												m_Specification.MipLevels,
												m_InternalFormat,
												m_Specification.Width,
												m_Specification.Height,
												m_Specification.Depth));
				break;
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TextureStorage3DMultisample(m_Handle,
														   m_Specification.Samples,
														   m_InternalFormat,
														   m_Specification.Width,
														   m_Specification.Height,
														   m_Specification.Depth,
														   GL_TRUE));
				break;
	#else
				throw std::runtime_error("Multisampled textures are not supported by WebGL");
	#endif
		}
	}

	void TextureOpenGL::CreateTextureFacesNonDSA(const GladGLContext &context)
	{
		glCall(context.GenTextures(1, &m_Handle));
		glCall(context.BindTexture(m_TextureType, m_Handle));
		glCall(context.PixelStorei(GL_PACK_ALIGNMENT, 1));
		glCall(context.PixelStorei(GL_UNPACK_ALIGNMENT, 1));
		glCall(context.TexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCall(context.TexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		switch (m_GLInternalTextureFormat)
		{
			case GL::GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TexStorage1D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width));
				break;
	#else
				throw std::runtime_error("1D textures are not supported by WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(
					context.TexStorage2D(m_TextureType, m_Specification.MipLevels, m_InternalFormat, m_Specification.Width, m_Specification.Height));
				break;
			case GL::GLInternalTextureFormat::Texture2DMultisample:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TexStorage2DMultisample(m_TextureType,
													   m_Specification.Samples,
													   m_InternalFormat,
													   m_Specification.Width,
													   m_Specification.Height,
													   GL_TRUE));
				break;
	#else
				throw std::runtime_error("Multisampled textures are not supported by WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
				glCall(context.TexStorage3D(m_TextureType,
											m_Specification.MipLevels,
											m_InternalFormat,
											m_Specification.Width,
											m_Specification.Height,
											m_Specification.ArrayLayers));
				break;
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TexStorage3DMultisample(m_TextureType,
													   m_Specification.Samples,
													   m_InternalFormat,
													   m_Specification.Width,
													   m_Specification.Height,
													   m_Specification.ArrayLayers,
													   GL_TRUE));
				break;
	#else
				throw std::runtime_error("Multisampled textures are not supported by WebGL");
	#endif
		}
	}

	GL::GLInternalTextureFormat TextureOpenGL::GetInternalGLTextureFormat() const
	{
		return m_GLInternalTextureFormat;
	}
}	 // namespace Nexus::Graphics

#endif