#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/UI/Form.hpp"

#include "Nexus-Core/UI/HorizontalSizer.hpp"
#include "Nexus-Core/UI/Panel.hpp"
#include "Nexus-Core/UI/UIRenderer.hpp"

namespace Nexus::UI
{
	class UIManager : public Nexus::Application
	{
	  public:
		UIManager(const Nexus::ApplicationSpecification &spec) : Application(spec)
		{
		}

		virtual ~UIManager()
		{
		}

		void Load() final
		{
			// m_MainForm = new Form(GetPrimaryWindow());

			m_Sizer		  = std::make_unique<Nexus::UI::HorizontalSizer>();
			m_Renderer	  = std::make_unique<Nexus::UI::UIRenderer>(m_GraphicsDevice);
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			m_Sizer->SetPosition({0, 0});
			m_Sizer->SetSize(GetWindowSize());
			m_Sizer->SetBackgroundColour({1.0f, 0.0f, 0.0f, 1.0f});

			/*Nexus::UI::Panel *panel2 = new Nexus::UI::Panel();
			panel2->SetPosition({10, 10});
			panel2->SetSize({150, 150});
			panel2->SetBackgroundColour({0.0f, 1.0f, 0.0f, 1.0f});
			panel2->SetRounding(2.0f);
			m_Panel->AddChild(panel2);

			Nexus::UI::Panel *panel3 = new Nexus::UI::Panel();
			panel3->SetPosition({10, 10});
			panel3->SetSize({150, 150});
			panel3->SetBackgroundColour({0.0f, 0.0f, 1.0f, 1.0f});
			panel3->SetRounding(15.0f);
			panel2->AddChild(panel3); */

			Nexus::UI::Panel *panel = new Nexus::UI::Panel();
			panel->SetPosition({10, 10});
			panel->SetSize({150, 150});
			panel->SetBackgroundColour({0.0f, 1.0f, 0.0f, 1.0f});
			panel->SetRounding(8.0f);
			m_Sizer->AddChild(panel);

			Nexus::UI::Panel *panel2 = new Nexus::UI::Panel();
			panel2->SetPosition({10, 10});
			panel2->SetSize({150, 150});
			panel2->SetBackgroundColour({0.0f, 0.0f, 1.0f, 1.0f});
			panel2->SetRounding(8.0f);
			m_Sizer->AddChild(panel2);

			GetPrimaryWindow()->SetExposeCallback(
				[&]()
				{
					m_Sizer->SetSize(GetWindowSize());

					Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();
					m_Renderer->Render(m_Sizer.get());
					Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers();
				});
			GetPrimaryWindow()->SetResizeCallback(
				[&](const WindowResizedEventArgs &args)
				{
					m_Sizer->SetSize(GetWindowSize());

					Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();
					m_Renderer->Render(m_Sizer.get());
					Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers();
				});

			/* Nexus::UI::Sizer *sizer = new Nexus::UI::HorizontalSizer();
			sizer->SetPosition({10, 10});
			sizer->SetSize({150, 150});
			sizer->SetBackgroundColour({1.0f, 1.0f, 1.0f, 1.0f});
			sizer->SetRounding(15.0f);
			panel3->AddChild(sizer); */
		}

		void Render(Nexus::TimeSpan time) final
		{
		}

		void Update(Nexus::TimeSpan time) final
		{
		}

		void Unload() final
		{
		}

		Form *GetMainForm()
		{
			return m_MainForm;
		}

	  private:
		Ref<Graphics::CommandList> m_CommandList = nullptr;
		Form					  *m_MainForm	 = nullptr;

		std::unique_ptr<Nexus::UI::HorizontalSizer> m_Sizer	   = nullptr;
		std::unique_ptr<Nexus::UI::UIRenderer>		m_Renderer = nullptr;
	};
}	 // namespace Nexus::UI
