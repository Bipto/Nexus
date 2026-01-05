#pragma once

#include "Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp"
#include "Nexus.hpp"
#include "Platform/FileSystem/FileSystem.hpp"
#include "Platform/Input/Events.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Demos
{
	class Demo
	{
	  public:
		explicit Demo(const std::string							&name,
					  Nexus::Application						*app,
					  Nexus::ImGuiUtils::ImGuiGraphicsRenderer	*imGuiRenderer,
					  Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: m_Name(name),
			  m_GraphicsDevice(app->GetGraphicsDevice()),
			  m_AudioDevice(app->GetAudioDevice()),
			  m_Window(app->GetPrimaryWindow()),
			  m_ImGuiRenderer(imGuiRenderer),
			  m_CommandQueue(commandQueue)
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

		virtual std::string GetInfo() const
		{
			return "";
		}

		const std::string &GetName() const
		{
			return m_Name;
		}

	  protected:
		std::string								   m_Name;
		Nexus::Graphics::IGraphicsDevice		  *m_GraphicsDevice = nullptr;
		Nexus::Ref<Nexus::Graphics::ICommandQueue> m_CommandQueue	= nullptr;
		Nexus::Audio::AudioDevice				  *m_AudioDevice	= nullptr;
		Nexus::IWindow							  *m_Window			= nullptr;
		Nexus::ImGuiUtils::ImGuiGraphicsRenderer  *m_ImGuiRenderer	= nullptr;
	};
}	 // namespace Demos