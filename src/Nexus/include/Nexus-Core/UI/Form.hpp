#pragma once

#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::UI
{
	class Form
	{
	  public:
		Form(const std::string &title, uint32_t width, uint32_t height)
		{
			Nexus::WindowSpecification spec;
			spec.Title	   = title;
			spec.Width	   = width;
			spec.Height	   = height;
			spec.Shown	   = false;
			spec.Resizable = false;
			m_Window	   = Platform::CreatePlatformWindow(spec);
		}

		Form(IWindow *window) : m_Window(window)
		{
		}

		void Show()
		{
			m_Window->Show();
		}

	  private:
		IWindow *m_Window = nullptr;
	};
}	 // namespace Nexus::UI