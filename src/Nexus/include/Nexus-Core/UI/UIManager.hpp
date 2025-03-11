#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/UI/Form.hpp"

#include "Nexus-Core/UI/HorizontalSizer.hpp"
#include "Nexus-Core/UI/Panel.hpp"
#include "Nexus-Core/UI/UIRenderer.hpp"
#include "Nexus-Core/UI/VerticalSizer.hpp"

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

			m_Sizer		  = std::make_unique<Nexus::UI::VerticalSizer>();
			m_Renderer	  = std::make_unique<Nexus::UI::UIRenderer>(m_GraphicsDevice);
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			m_Sizer->SetPosition({0, 0});
			m_Sizer->SetSize(GetWindowSize());
			m_Sizer->SetBackgroundColour({1.0f, 0.0f, 0.0f, 1.0f});
			m_Sizer->SetRows({SizePercentage(50.0f), SizeAbsolute(100)});

			Nexus::UI::HorizontalSizer *hSizer = new Nexus::UI::HorizontalSizer();
			hSizer->SetPosition({10, 10});
			hSizer->SetSize({500, 500});
			hSizer->SetBackgroundColour({1.0f, 1.0f, 1.0f, 1.0f});
			hSizer->SetRounding(8.0f);
			hSizer->SetColumns({SizePercentage(100.0f)});
			m_Sizer->AddChild(hSizer);

			Nexus::UI::Panel *panel = new Nexus::UI::Panel();
			panel->SetPosition({0, 0});
			panel->SetSize({150, 150});
			panel->SetBackgroundColour({1.0f, 1.0f, 0.0f, 1.0f});
			panel->SetRounding(8.0f);
			hSizer->AddChild(panel);

			Nexus::UI::HorizontalSizer *sizer2 = new Nexus::UI::HorizontalSizer();
			sizer2->SetPosition({10, 10});
			sizer2->SetSize({150, 150});
			sizer2->SetBackgroundColour({0.0f, 0.0f, 1.0f, 1.0f});
			sizer2->SetRounding(8.0f);
			m_Sizer->AddChild(sizer2);

			GetPrimaryWindow()->SetExposeCallback(
				[&]()
				{
					m_Sizer->SetSize(GetWindowSize());

					Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();
					m_Renderer->Render(m_Sizer.get());
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
			return m_MainForm;
		}

	  private:
		Ref<Graphics::CommandList> m_CommandList = nullptr;
		Form					  *m_MainForm	 = nullptr;

		std::unique_ptr<Nexus::UI::VerticalSizer>	m_Sizer	   = nullptr;
		std::unique_ptr<Nexus::UI::UIRenderer>		m_Renderer = nullptr;
	};
}	 // namespace Nexus::UI
