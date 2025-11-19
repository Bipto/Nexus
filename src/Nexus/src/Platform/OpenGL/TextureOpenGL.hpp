#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "DeviceBufferOpenGL.hpp"
	#include "GL.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;
	class TextureViewOpenGL;

	class TextureOpenGL : public ITexture
	{
	  public:
		TextureOpenGL(const TextureDescription &spec, GraphicsDeviceOpenGL *graphicsDevice);
		virtual ~TextureOpenGL();
		TextureLayout		 GetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel) const final;
		void				 SetTextureLayout(uint32_t arrayLayer, uint32_t mipLevel, TextureLayout layout);
		SubresourceFootprint GetSubresourceFootprint(uint32_t arrayLayer, uint32_t mipLevel) const final;

		void	 Bind(uint32_t slot) const;
		uint32_t GetHandle() const;
		GLenum	 GetTextureType() const;
		GLenum	 GetDataFormat() const;
		GLenum	 GetBaseType() const;

		GL::GLInternalTextureFormat GetInternalGLTextureFormat() const;

		void AddView(WeakRef<TextureViewOpenGL> view);
		void MarkDirty();

	  private:
		void CreateTextureFacesDSA(const GladGLContext &context);
		void CreateTextureFacesNonDSA(const GladGLContext &context);

	  private:
		GraphicsDeviceOpenGL *m_Device = nullptr;

		uint32_t m_Handle		  = 0;
		GLenum	 m_DataFormat	  = 0;
		GLenum	 m_InternalFormat = 0;
		GLenum	 m_BaseType		  = 0;
		GLenum	 m_TextureType	  = 0;

		uint32_t					m_Framebuffer			  = 0;
		GL::GLInternalTextureFormat m_GLInternalTextureFormat = GL::GLInternalTextureFormat::Texture2D;
		std::vector<TextureLayout>	m_TextureLayout			  = {};

		std::vector<WeakRef<TextureViewOpenGL>> m_Views = {};
	};
}	 // namespace Nexus::Graphics

#endif