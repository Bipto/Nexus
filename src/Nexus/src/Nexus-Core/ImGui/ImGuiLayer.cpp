#include "Nexus-Core/ImGui/ImGuiLayer.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Platform/Events/EventDispatcher.hpp"

namespace Nexus
{
	ImGuiLayer::ImGuiLayer(Nexus::Application *app, Nexus::Ref<Graphics::ICommandQueue> commandQueue)
		: m_ImGuiRenderer(std::make_unique<ImGuiUtils::ImGuiGraphicsRenderer>(app, commandQueue)),
		  m_CommandQueue(commandQueue)
	{
		ImGuiContext *context = m_ImGuiRenderer->GetContext();
		ImGui::SetCurrentContext(context);
		m_ImGuiRenderer->RebuildFontAtlas();

		m_CommandList = m_CommandQueue->CreateCommandList();
	}

	bool ImGuiLayer::OnEvent(const Event &event)
	{
		/*EventDispatcher dispatcher = {};
		dispatcher.Subscribe<TextInputEventArgs>([this](const TextInputEventArgs &args) { m_ImGuiRenderer->AddTextInput(args); });
		dispatcher.Subscribe<MouseScrolledEventArgs>([this](const MouseScrolledEventArgs &args) { m_ImGuiRenderer->AddMouseScroll(args); });
		dispatcher.Subscribe<KeyPressedEventArgs>([this](const KeyPressedEventArgs &args) { m_ImGuiRenderer->AddKeyPressed(args); });
		dispatcher.Subscribe<KeyReleasedEventArgs>([this](const KeyReleasedEventArgs &args) { m_ImGuiRenderer->AddKeyReleased(args); });

		if (ImGui::IsWindowHovered())
		{
			return true;
		}*/

		return false;
	}

	void ImGuiLayer::OnRender(Nexus::TimeSpan time, IWindow *window)
	{
		m_CommandList->Begin();

		Nexus::Ref<Nexus::Graphics::ISwapchain>	  swapchain	  = Nexus::GetApplication()->GetPrimarySwapchain();
		Nexus::Ref<Nexus::Graphics::IFramebuffer> framebuffer = swapchain->GetCurrentFramebuffer();
		m_CommandList->SetFramebuffer(framebuffer);
		m_CommandList->ClearColourTarget(0, {0.35f, 0.25f, 0.42f, 1.0f});

		m_CommandList->End();

		m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);

		m_ImGuiRenderer->BeforeLayout(time);
		OnImGuiRenderer();
		m_ImGuiRenderer->AfterLayout();

		swapchain->SwapBuffers({});
	}

	void ImGuiLayer::OnUpdate(Nexus::TimeSpan time, IWindow *window)
	{
	}

	void ImGuiLayer::OnTick(Nexus::TimeSpan time, IWindow *window)
	{
	}
}	 // namespace Nexus