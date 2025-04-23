#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/UI/Form.hpp"

#include "Nexus-Core/UI/Button.hpp"
#include "Nexus-Core/UI/GridSizer.hpp"
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
			m_Sizer		  = std::make_unique<Nexus::UI::GridSizer>();
			m_Renderer	  = std::make_unique<Nexus::UI::UIRenderer>(m_GraphicsDevice.get());
			m_CommandList = m_GraphicsDevice->CreateCommandList();

			m_Sizer->SetPosition({0, 0});
			m_Sizer->SetSize(GetWindowSize());
			m_Sizer->SetBackgroundColour({1.0f, 0.0f, 0.0f, 1.0f});
			m_Sizer->SetLayout(9, 9);

			for (size_t i = 0; i < 81; i++)
			{
				Nexus::UI::Button *button = new Nexus::UI::Button();
				button->SetBackgroundColour(Utils::GenerateRandomColour());
				button->SetSize({100, 100});
				button->SetName(std::to_string(i));

				button->SetMousePressedCallback([&](const Nexus::MouseButtonPressedEventArgs &args, Control *control)
												{ std::cout << "Button " << control->GetName() << " clicked" << std::endl; });
				m_Sizer->AddChild(button);
			}

			GetPrimaryWindow()->SetRenderFunction(
				[&](TimeSpan time)
				{
					m_Sizer->SetSize(GetWindowSize());

					Nexus::GetApplication()->GetPrimarySwapchain()->Prepare();
					m_Renderer->Render(m_Sizer.get());
					Nexus::GetApplication()->GetPrimarySwapchain()->SwapBuffers();
				});

			GetPrimaryWindow()->AddMouseMovedCallback([&](const MouseMovedEventArgs &args) { m_Sizer->InvokeOnMouseMoved(args); });
			GetPrimaryWindow()->AddMousePressedCallback([&](const MouseButtonPressedEventArgs &args) { m_Sizer->InvokeOnMousePressed(args); });
			GetPrimaryWindow()->AddMouseReleasedCallback([&](const MouseButtonReleasedEventArgs &args) { m_Sizer->InvokeOnMouseReleased(args); });
			GetPrimaryWindow()->AddMouseScrollCallback([&](const MouseScrolledEventArgs &args) { m_Sizer->InvokeOnMouseScroll(args); });
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

		std::unique_ptr<Nexus::UI::GridSizer>  m_Sizer	  = nullptr;
		std::unique_ptr<Nexus::UI::UIRenderer> m_Renderer = nullptr;
	};
}	 // namespace Nexus::UI
