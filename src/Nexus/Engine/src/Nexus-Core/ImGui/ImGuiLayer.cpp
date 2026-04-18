#include "Nexus-Core/ImGui/ImGuiLayer.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Platform/Events/EventDispatcher.hpp"

namespace Nexus
{
	ImGuiLayer::ImGuiLayer(Nexus::Application *app, Graphics::CommandQueueHandle commandQueue)
		: m_ImGuiRenderer(std::make_unique<ImGuiUtils::ImGuiGraphicsRenderer>(app, commandQueue)),
		  m_CommandQueue(commandQueue),
		  m_Application(app)
	{
		ImGuiContext *context = m_ImGuiRenderer->GetContext();
		ImGui::SetCurrentContext(context);
		m_ImGuiRenderer->RebuildFontAtlas();

		m_CommandList = m_CommandQueue->CreateCommandList();

		ImGuiIO &io = m_ImGuiRenderer->GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	bool ImGuiLayer::OnEvent(const Event &event)
	{
		EventDispatcher dispatcher = {};
		dispatcher.Subscribe<TextInputEventArgs>([this](const TextInputEventArgs &args) { m_ImGuiRenderer->AddTextInput(args); });
		dispatcher.Subscribe<MouseScrolledEventArgs>([this](const MouseScrolledEventArgs &args) { m_ImGuiRenderer->AddMouseScroll(args); });
		dispatcher.Subscribe<KeyPressedEventArgs>([this](const KeyPressedEventArgs &args) { m_ImGuiRenderer->AddKeyPressed(args); });
		dispatcher.Subscribe<KeyReleasedEventArgs>([this](const KeyReleasedEventArgs &args) { m_ImGuiRenderer->AddKeyReleased(args); });
		dispatcher.Subscribe<MouseMovedEventArgs>([this](const MouseMovedEventArgs &args) { m_ImGuiRenderer->AddMouseMoved(args); });
		dispatcher.Subscribe<MouseButtonPressedEventArgs>([this](const MouseButtonPressedEventArgs &args)
														  { m_ImGuiRenderer->AddMouseButtonPressed(args); });
		dispatcher.Subscribe<MouseButtonReleasedEventArgs>([this](const MouseButtonReleasedEventArgs &args)
														   { m_ImGuiRenderer->AddMouseButtonReleased(args); });

		dispatcher.Dispatch(event);

		if (m_IsAnyWindowHovered)
		{
			return true;
		}

		return false;
	}

	void ImGuiLayer::OnRender(Nexus::TimeSpan time, IWindow *window)
	{
		Graphics::SwapchainHandle swapchain = Nexus::GetApplication()->GetPrimarySwapchain();

		m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);

		m_ImGuiRenderer->BeforeLayout(time);
		OnImGuiRenderer();
		m_IsAnyWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered();
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