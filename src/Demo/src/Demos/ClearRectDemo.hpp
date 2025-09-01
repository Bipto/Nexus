#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"

namespace Demos
{
	class ClearRectDemo : public Demo
	{
	  public:
		ClearRectDemo(const std::string							&name,
					  Nexus::Application						*app,
					  Nexus::ImGuiUtils::ImGuiGraphicsRenderer	*imGuiRenderer,
					  Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~ClearRectDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			NX_PROFILE_FUNCTION();

			{
				NX_PROFILE_SCOPE("Command recording");

				Nexus::IWindow *window		= Nexus::GetApplication()->GetPrimaryWindow();
				int32_t			clearWidth	= window->GetWindowSize().X / 2;
				int32_t			clearHeight = window->GetWindowSize().Y / 2;

				m_CommandList->Begin();
				m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});

				{
					Nexus::Graphics::ClearRect clearRect = {};
					clearRect.X							 = 0;
					clearRect.Y							 = 0;
					clearRect.Width						 = clearWidth;
					clearRect.Height					 = clearHeight;
					m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f}, clearRect);
				}

				{
					Nexus::Graphics::ClearRect clearRect = {};
					clearRect.X							 = clearWidth;
					clearRect.Y							 = 0;
					clearRect.Width						 = clearWidth;
					clearRect.Height					 = clearHeight;
					m_CommandList->ClearColorTarget(0, {1.0f, 0.0f, 0.0f, 1.0f}, clearRect);
				}

				{
					Nexus::Graphics::ClearRect clearRect = {};
					clearRect.X							 = 0;
					clearRect.Y							 = clearHeight;
					clearRect.Width						 = clearWidth;
					clearRect.Height					 = clearHeight;
					m_CommandList->ClearColorTarget(0, {0.0f, 1.0f, 0.0f, 1.0f}, clearRect);
				}

				{
					Nexus::Graphics::ClearRect clearRect = {};
					clearRect.X							 = clearWidth;
					clearRect.Y							 = clearHeight;
					clearRect.Width						 = clearWidth;
					clearRect.Height					 = clearHeight;
					m_CommandList->ClearColorTarget(0, {0.0f, 0.0f, 1.0f, 1.0f}, clearRect);
				}

				m_CommandList->End();
			}

			{
				NX_PROFILE_SCOPE("Command submission");
				m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
				m_GraphicsDevice->WaitForIdle();
			}
		}

		virtual void RenderUI() override
		{
			ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColour));
		}

		virtual std::string GetInfo() const override
		{
			return "Clearing the screen using a pickable colour";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};
	};
}	 // namespace Demos