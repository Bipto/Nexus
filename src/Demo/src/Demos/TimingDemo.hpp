#pragma once

#pragma once

#include "Demo.hpp"

namespace Demos
{
	class TimingDemo : public Demo
	{
	  public:
		TimingDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~TimingDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
			m_TimingQuery = m_GraphicsDevice->CreateTimingQuery();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CommandList->Begin();

			m_CommandList->StartTimingQuery(m_TimingQuery);

			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {Nexus::GetApplication()->GetPrimarySwapchain()});
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});

			m_CommandList->StopTimingQuery(m_TimingQuery);
			m_CommandList->End();

			m_GraphicsDevice->SubmitCommandList(m_CommandList);

			m_TimerCounter += time.GetSeconds();

			// update the timings every half a second
			if (m_TimerCounter >= 0.5f)
			{
				m_TimingQuery->Resolve();
				m_Timing	   = m_TimingQuery->GetElapsedMilliseconds();
				m_TimerCounter = 0.0f;
			}
		}

		virtual void RenderUI() override
		{
			ImGui::ColorEdit3("Clear Colour", glm::value_ptr(m_ClearColour));
			ImGui::Text("Time taken: %1.2f Ms", m_Timing);
		}

		virtual std::string GetInfo() const override
		{
			return "Inserting GPU timestamps into CommandList recording";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {0.7f, 0.2f, 0.3f};

		Nexus::Ref<Nexus::Graphics::TimingQuery> m_TimingQuery;
		float									 m_Timing		= 0;
		float									 m_TimerCounter = 1.0f;
	};
}	 // namespace Demos