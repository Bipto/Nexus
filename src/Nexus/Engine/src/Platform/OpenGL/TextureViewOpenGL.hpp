#pragma once

#include "GraphicsDeviceOpenGL.hpp"

#include "RHI/TextureView.hpp"

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
		void						  MarkDirty();

	  private:
		void CreateTextureView(const GladGLContext &context);
		void CreateEmulatedView(const GladGLContext &context);
		void UpdateEmulatedView(const GladGLContext &context);

	  private:
		TextureViewDescription m_Description		 = {};
		GraphicsDeviceOpenGL  *m_Device				 = nullptr;
		uint32_t			   m_Handle				 = 0;
		Ref<TextureOpenGL>	   m_EmulatedTextureView = nullptr;
		GLenum				   m_ViewType			 = 0;

		bool m_TextureViewRequired = false;
		bool m_Dirty			   = true;
	};
}	 // namespace Nexus::Graphics