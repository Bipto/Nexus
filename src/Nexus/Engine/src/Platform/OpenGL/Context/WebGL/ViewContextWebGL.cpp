#if defined(NX_PLATFORM_WEBGL)

	#include "ViewContextWebGL.hpp"
	#include "OffscreenContextWebGL.hpp"

	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/val.h>

	#include "Nexus-Core/Timings/Profiler.hpp"
	#include "Nexus-Core/Point.hpp"

namespace Nexus::GL
{
	BoundingClientRect GetBoundingClientRect(const std::string &id)
	{
		emscripten::val document = emscripten::val::global("document");
		emscripten::val element	 = document.call<emscripten::val>("getElementById", id);
		emscripten::val window	 = emscripten::val::global("window");

		if (element.isUndefined() || element.isNull())
		{
			std::cout << "Could not find element" << std::endl;
			return {};
		}
		emscripten::val rect = element.call<emscripten::val>("getBoundingClientRect");

		if (rect.isUndefined() || rect.isNull())
		{
			std::cout << "Could not get bounding client rect from element" << std::endl;
			return {};
		}

		double left		  = rect["x"].as<double>();
		double top		  = rect["y"].as<double>();
		double width	  = rect["width"].as<double>();
		double height	  = rect["height"].as<double>();

		return BoundingClientRect {.Left = left, .Top = top, .Width = width, .Height = height};
	}

	ViewContextWebGL::ViewContextWebGL(const std::string					 &canvasName,
									   Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice,
									   const ContextSpecification			 &spec)
		: m_Device(graphicsDevice),
		  m_Description(spec),
		  m_CanvasName(canvasName)
	{
		CreateFramebuffer();
	}

	ViewContextWebGL::~ViewContextWebGL()
	{
	}

	bool Nexus::GL::ViewContextWebGL::MakeCurrent()
	{
		Ref<Graphics::FramebufferOpenGL> framebufferOpenGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
		framebufferOpenGL->BindAsDrawBuffer();
		return true;
	}

	void Nexus::GL::ViewContextWebGL::Swap()
	{
		NX_PROFILE_FUNCTION();

		OffscreenContextWebGL *offscreenContext	   = (OffscreenContextWebGL *)m_Device->GetOffscreenContext();
		std::string			   offscreenCanvasName = offscreenContext->GetCanvasName();

		uint32_t textureWidth  = m_Framebuffer->GetFramebufferSpecification().Width;
		uint32_t textureHeight = m_Framebuffer->GetFramebufferSpecification().Height;

		Ref<Graphics::FramebufferOpenGL> framebufferOpenGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferOpenGL->GetHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		BoundingClientRect fullscreenRect = GetBoundingClientRect(offscreenCanvasName);
		BoundingClientRect viewRect		  = GetBoundingClientRect(m_CanvasName);

		float x		 = viewRect.Left;
		float y		 = fullscreenRect.Height - (viewRect.Top + viewRect.Height);
		float width	 = viewRect.Width;
		float height = viewRect.Height;

		glEnable(GL_SCISSOR_TEST);
		glViewport(0, 0, textureWidth, textureHeight);
		glScissor(x, y, textureWidth, textureHeight);

		glBlitFramebuffer(0, 0, textureWidth, textureHeight, x, y, x + textureWidth, y + textureHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		if (viewRect.Width != textureWidth || viewRect.Height != textureHeight)
		{
			CreateFramebuffer();
		}
	}

	void Nexus::GL::ViewContextWebGL::SetVSync(bool enabled)
	{
	}

	const ContextSpecification &Nexus::GL::ViewContextWebGL::GetDescription() const
	{
		return m_Description;
	}

	void Nexus::GL::ViewContextWebGL::HandleResize()
	{
		CreateFramebuffer();
	}

	void Nexus::GL::ViewContextWebGL::CreateFramebuffer()
	{
		BoundingClientRect rect = GetBoundingClientRect(m_CanvasName);

		Graphics::FramebufferSpecification framebufferSpec		 = {};
		framebufferSpec.Width									 = (uint32_t)rect.Width;
		framebufferSpec.Height									 = (uint32_t)rect.Height;

		std::cout << "Creating framebuffer: [Width: " << framebufferSpec.Width << ", Height: " << framebufferSpec.Height << "]" << std::endl;

		framebufferSpec.ColourAttachmentSpecification.Attachments = {Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
		framebufferSpec.DepthAttachmentSpecification			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;
		framebufferSpec.Samples									 = 1;
		m_Framebuffer											 = m_Device->CreateFramebuffer(framebufferSpec);
	}

	bool ViewContextWebGL::Validate()
	{
		return true;
	}

}	 // namespace Nexus::GL

#endif