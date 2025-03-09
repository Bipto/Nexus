#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/UI/Form.hpp"

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

			m_Panel		  = std::make_unique<Nexus::UI::Panel>();
			m_Renderer	  = std::make_unique<Nexus::UI::UIRenderer>(m_GraphicsDevice);
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			m_Panel->SetPosition({0, 0});
			m_Panel->SetSize(GetWindowSize());
			m_Panel->SetBackgroundColour({1.0f, 0.0f, 0.0f, 1.0f});

			Nexus::UI::Panel *panel2 = new Nexus::UI::Panel();
			panel2->SetPosition({200, 200});
			panel2->SetSize({150, 150});
			panel2->SetBackgroundColour({0.0f, 1.0f, 0.0f, 1.0f});
			panel2->SetRounding(15.0f);
			m_Panel->AddChild(panel2);

			Nexus::UI::Panel *panel3 = new Nexus::UI::Panel();
			panel3->SetPosition({225, 225});
			panel3->SetSize({150, 150});
			panel3->SetBackgroundColour({0.0f, 0.0f, 1.0f, 1.0f});
			panel3->SetRounding(15.0f);
			panel2->AddChild(panel3);

			GetPrimaryWindow()->SetExposeCallback(
				[&]()
				{
					m_Panel->SetSize(GetWindowSize());

					Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();
					m_Renderer->Render(m_Panel.get());
					Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers();
				});
			GetPrimaryWindow()->SetResizeCallback(
				[&](const WindowResizedEventArgs &args)
				{
					m_Panel->SetSize(GetWindowSize());

					Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();
					m_Renderer->Render(m_Panel.get());
					Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers();
				});
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
		}

	  private:
		Ref<Graphics::CommandList> m_CommandList = nullptr;
		Form					  *m_MainForm	 = nullptr;

		std::unique_ptr<Nexus::UI::Panel>	   m_Panel	  = nullptr;
		std::unique_ptr<Nexus::UI::UIRenderer> m_Renderer = nullptr;
	};
}	 // namespace Nexus::UI
