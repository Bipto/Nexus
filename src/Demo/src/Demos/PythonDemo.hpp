#pragma once

#include "Demo.hpp"
#include "pocketpy.h"

namespace Demos
{
	class PythonDemo : public Demo
	{
	  public:
		PythonDemo(const std::string &name, Nexus::Application *app, Nexus::ImGuiUtils::ImGuiGraphicsRenderer *imGuiRenderer)
			: Demo(name, app, imGuiRenderer)
		{
		}

		virtual ~PythonDemo()
		{
		}

		virtual void Load() override
		{
			m_CommandList = m_GraphicsDevice->CreateCommandList();
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CommandList->Begin();
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget {m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain()});
			m_CommandList->ClearColorTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			m_CommandList->End();
			m_GraphicsDevice->SubmitCommandList(m_CommandList);
		}

		virtual void RenderUI() override
		{
			ImGui::Separator();
			ImGui::InputTextMultiline("Python Script: ", &m_PythonScript);
			if (ImGui::Button("Execute"))
			{
				pkpy::VM *vm = new pkpy::VM();
				vm->exec(m_PythonScript);
				delete vm;
			}
		}

		virtual std::string GetInfo() const override
		{
			return "Customisable scripting using Python.";
		}

	  private:
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList;
		glm::vec3								 m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		std::string m_PythonScript = "print('Hello from Python')";
	};
}	 // namespace Demos