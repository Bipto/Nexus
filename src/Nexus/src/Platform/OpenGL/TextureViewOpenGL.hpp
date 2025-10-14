#pragma once

#include "GraphicsDeviceOpenGL.hpp"

#include "Nexus-Core/Graphics/TextureView.hpp"

namespace Nexus::Graphics
{
	class TextureViewOpenGL : public ITextureView
	{
	  public:
		TextureViewOpenGL(const TextureViewDescription &desc, GraphicsDeviceOpenGL *device);
		virtual ~TextureViewOpenGL();
		const TextureViewDescription &GetDescription() const final;
		uint32_t					  GetHandle() const;
		void						  Bind(uint32_t slot);

	  private:
		TextureViewDescription m_Description = {};
		GraphicsDeviceOpenGL  *m_Device		 = nullptr;
		uint32_t			   m_Handle		 = 0;
		GLenum				   m_ViewType	 = 0;
	};
}	 // namespace Nexus::Graphics