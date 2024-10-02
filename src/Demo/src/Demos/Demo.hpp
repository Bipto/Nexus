#pragma once

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus-Core/Input/InputEvent.hpp"
#include "Nexus.hpp"

namespace Demos
{
	class Demo
	{
	  public:
		explicit Demo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: m_Name(name),
			  m_GraphicsDevice(app->GetGraphicsDevice()),
			  m_AudioDevice(app->GetAudioDevice()),
			  m_Window(app->GetPrimaryWindow()),
			  m_ImGuiRenderer(imGuiRenderer)
		{
		}

		virtual ~Demo()
		{
		}

		virtual void Load()
		{
		}

		virtual void Update(Nexus::TimeSpan time)
		{
		}
		virtual void Render(Nexus::TimeSpan time)
		{
		}

		virtual void OnResize(Nexus::Point2D<uint32_t> size)
		{
		}

		virtual void RenderUI()
		{
		}

		virtual void OnEvent(const Nexus::InputEvent &event)
		{
		}

		const std::string &GetName() const
		{
			return m_Name;
		}

	  protected:
		std::string								  m_Name;
		Nexus::Graphics::GraphicsDevice			 *m_GraphicsDevice = nullptr;
		Nexus::Audio::AudioDevice				 *m_AudioDevice	   = nullptr;
		Nexus::Window							 *m_Window		   = nullptr;
		Nexus::ImGuiUtils::ImGuiGraphicsRenderer *m_ImGuiRenderer  = nullptr;
	};
}	 // namespace Demos