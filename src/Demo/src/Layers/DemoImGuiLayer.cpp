#include "DemoImGuiLayer.hpp"

#include "../Demos/AudioDemo.hpp"
#include "../Demos/BatchingDemo.hpp"
#include "../Demos/CameraDemo.hpp"
#include "../Demos/ClearRectDemo.hpp"
#include "../Demos/ClearScreenDemo.hpp"
#include "../Demos/ClippingAndTriangulationDemo.hpp"
#include "../Demos/ComputeDemo.hpp"
#include "../Demos/ComputeIndirectDemo.hpp"
#include "../Demos/CubemapDemo.hpp"
#include "../Demos/Demo3D.hpp"
#include "../Demos/FramebufferDemo.hpp"
#include "../Demos/GeometryShaderDemo.hpp"
#include "../Demos/HelloTriangle.hpp"
#include "../Demos/HelloTriangleIndexed.hpp"
#include "../Demos/HelloTriangleIndirect.hpp"
#include "../Demos/HelloTriangleIndirectIndexedDemo.hpp"
#include "../Demos/HelloTriangleMeshShaders.hpp"
#include "../Demos/HelloTriangleMeshShadersIndirect.hpp"
#include "../Demos/InstancingDemo.hpp"
#include "../Demos/Lighting.hpp"
#include "../Demos/MipmapDemo.hpp"
#include "../Demos/Models.hpp"
#include "../Demos/RayTracingDemo.hpp"
#include "../Demos/Splines.hpp"
#include "../Demos/StorageBufferDemo.hpp"
#include "../Demos/Texturing.hpp"
#include "../Demos/TimingDemo.hpp"
#include "../Demos/UniformBufferDemo.hpp"

#include <format>

namespace
{
    std::string GraphicsAPIToString(Nexus::Graphics::GraphicsAPI api)
    {
        switch (api)
        {
        case Nexus::Graphics::GraphicsAPI::OpenGL:
            return "OpenGL";
        case Nexus::Graphics::GraphicsAPI::D3D12:
            return "DirectX12";
        case Nexus::Graphics::GraphicsAPI::Vulkan:
            return "Vulkan";
        default:
            throw std::runtime_error("Failed to find a valid graphics API");
        }
    }
} // namespace

DemoImGuiLayer::DemoImGuiLayer(Nexus::Application *app, Nexus::Graphics::CommandQueueHandle commandQueue)
    : Nexus::ImGuiLayer(app, commandQueue), m_GraphicsDevice(commandQueue->GetGraphicsDevice())
{
    ImGuiContext *context = m_ImGuiRenderer->GetContext();
    ImGui::SetCurrentContext(context);
    ImGui::GetStyle().ScrollbarSize = 20.0f;

    ImGuiIO &io = m_ImGuiRenderer->GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    int size = 20;

#if defined(__ANDROID__) || defined(ANDROID)
    size = 42;
#endif

    std::string fontPath = Nexus::FileSystem::GetFilePathAbsolute("resources/demo/fonts/roboto/roboto-regular.ttf");
    io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), size);
    m_ImGuiRenderer->RebuildFontAtlas();

    RegisterDemo<Demos::ClearScreenDemo>("Graphics", "Clear Colour");
    RegisterDemo<Demos::ClearRectDemo>("Graphics", "Clear Rects");
    RegisterDemo<Demos::TimingDemo>("Graphics", "Timings");
    RegisterDemo<Demos::HelloTriangleDemo>("Graphics", "Hello Triangle");
    RegisterDemo<Demos::HelloTriangleIndexedDemo>("Graphics", "Hello Triangle Indexed");
    RegisterDemo<Demos::HelloTriangleIndirectDemo>("Graphics", "Hello Triangle Indirect");
    RegisterDemo<Demos::HelloTriangleIndirectIndexedDemo>("Graphics", "Hello Triangle Indexed Indirect");

    const Nexus::Graphics::DeviceFeatures deviceFeatures = m_GraphicsDevice->GetPhysicalDeviceFeatures();
    if (deviceFeatures.SupportsMeshTaskShaders)
    {
        RegisterDemo<Demos::HelloTriangleMeshShadersDemo>("Graphics", "Hello Triangle Mesh Shaders");
        RegisterDemo<Demos::HelloTriangleMeshShadersIndirect>("Graphics", "Hello Triangle Mesh Shaders Indirect");
    }

    RegisterDemo<Demos::TexturingDemo>("Graphics", "Texturing");
    RegisterDemo<Demos::BatchingDemo>("Graphics", "Batching");
    RegisterDemo<Demos::FramebufferDemo>("Graphics", "Framebuffers");
    RegisterDemo<Demos::UniformBufferDemo>("Graphics", "Uniform Buffers");
    RegisterDemo<Demos::StorageBufferDemo>("Graphics", "Storage Buffers");
    RegisterDemo<Demos::Demo3D>("Graphics", "3D");
    RegisterDemo<Demos::CameraDemo>("Graphics", "Camera");
    RegisterDemo<Demos::LightingDemo>("Graphics", "Lighting");
    RegisterDemo<Demos::ModelDemo>("Graphics", "Models");
    RegisterDemo<Demos::InstancingDemo>("Graphics", "Instancing");
    RegisterDemo<Demos::MipmapDemo>("Graphics", "Mipmaps");
    RegisterDemo<Demos::CubemapDemo>("Graphics", "Cubemaps");
    RegisterDemo<Demos::ComputeDemo>("Graphics", "Compute");
    RegisterDemo<Demos::ComputeIndirectDemo>("Graphics", "Compute Indirect");

    // geometry shaders have some issues with SPIRV-Cross HLSL backend
    if (m_GraphicsDevice->GetGraphicsAPI().API != Nexus::Graphics::GraphicsAPI::D3D12)
    {
        RegisterDemo<Demos::GeometryShaderDemo>("Graphics", "Geometry Shader");
    }

    RegisterDemo<Demos::RayTracingDemo>("Graphics", "Ray Tracing");
    RegisterDemo<Demos::AudioDemo>("Audio", "Audio");
    RegisterDemo<Demos::ClippingAndTriangulationDemo>("Utils", "Polygon clipping and triangulation");
    RegisterDemo<Demos::Splines>("Utils", "Splines");

    /*Nexus::UI::IFrame *frame = m_Layout.CreateFrame("ImGui Frame");
    Nexus::UI::IMenubar *menubar = frame->CreateMenubar();
    Nexus::UI::IMenu *fileMenu = menubar->CreateMenu("File");

    Nexus::UI::IMenu *newMenu = fileMenu->AppendSubMenu("New");
    Nexus::UI::IMenuItem *newFileItem = newMenu->Append("Item");

    Nexus::UI::IMenuItem *separator = fileMenu->AppendSeparator();

    Nexus::UI::IMenuItem *closeItem = fileMenu->Append("Close");
    closeItem->OnClick([&]() { m_Application->Close(); });*/

    // Nexus::UI::IStatusBar *statusBar = frame->CreateStatusbar();
    // statusBar->SetStatusText("Hello from ImGui");

    /*Nexus::UI::IPanel *panel = m_Layout.CreatePanel();
    panel->SetPosition({500, 500});
    panel->SetSize({300, 300});

    Nexus::UI::IButton *button = panel->CreateButton("Click Me", Nexus::UI::Position
    {100, 100}, Nexus::UI::Size {250, 100}); button->OnClick(
        []()
        {
            auto messageBoxDesc			= Nexus::MessageBoxDescription {};
            messageBoxDesc.Buttons		= {Nexus::MessageBoxButton {.Key =
    Nexus::DefaultKey::Escape, .ID = 0, .Text = "Close"}};
            messageBoxDesc.ParentWindow =
    Nexus::GetApplication()->GetPrimaryWindow(); auto messageBox				=
    Nexus::Platform::CreateMessageBox(messageBoxDesc); messageBox->Show();
        });*/

    // update framerate counter once every second
    m_FramerateUpdateTimer.Every([this](Nexus::TimeSpan timespan) {
        // std::stringstream ss;
        // float fps = ImGui::GetIO().Framerate;
        // ss << "Running at: " << std::to_string(fps) << " FPS";

        // m_FramerateString = ss.str();

        m_FramerateString = std::format("Running at {} FPS", static_cast<uint32_t>(ImGui::GetIO().Framerate));
    }, 0.5);
}

void DemoImGuiLayer::OnImGuiRenderer()
{
    {
        NX_PROFILE_SCOPE("Render UI");
        m_Layout.Render();
        ImGui::Begin("Demos");
        RenderDemoInfo();
        RenderPerformanceInfo();
        ImGui::End();

        m_FramerateUpdateTimer.Update();
    }
}

void DemoImGuiLayer::SetDemoSelectedCallback(std::function<void(std::shared_ptr<Demos::Demo>)> function)
{
    m_CallbackFunction = function;
}

void DemoImGuiLayer::RenderDemoList()
{
    NX_PROFILE_FUNCTION();

    for (const auto &[menuName, demoList] : m_DemoInfos)
    {
        if (ImGui::TreeNodeEx(menuName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
        {
            for (const auto &pair : demoList)
            {
                auto flags =
                    ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;

                if (ImGui::TreeNodeEx(pair.Name.c_str(), flags))
                {
                    if (ImGui::IsItemClicked())
                    {
                        m_CurrentDemo = std::shared_ptr<Demos::Demo>(
                            pair.CreationFunction(m_Application, pair.Name, m_ImGuiRenderer.get(), m_CommandQueue));
                        m_CurrentDemo->Load();

                        if (m_CallbackFunction)
                        {
                            m_CallbackFunction(m_CurrentDemo);
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}

void DemoImGuiLayer::RenderDemoInfo()
{
    NX_PROFILE_FUNCTION();

    if (m_CurrentDemo)
    {
        if (ImGui::Button("<- Back"))
        {
            m_CurrentDemo = nullptr;

            if (m_CallbackFunction)
            {
                m_CallbackFunction(m_CurrentDemo);
            }
        }

        // required because demo could be deleted in the previous if statement
        if (m_CurrentDemo)
        {
            // render demo name
            std::string label = std::string("Selected Demo - ") + m_CurrentDemo->GetName();
            ImGui::Text("%s", label.c_str());

            const std::string &info = m_CurrentDemo->GetInfo();
            if (!info.empty())
            {
                std::string description = std::string("Description: ") + info;
                ImGui::Text("%s", description.c_str());
            }

            ImGui::Separator();

            std::shared_ptr<Nexus::Graphics::IPhysicalDevice> physicalDevice = m_GraphicsDevice->GetPhysicalDevice();

            Nexus::Graphics::GraphicsAPIInfo apiInfo = m_GraphicsDevice->GetGraphicsAPI();
            std::string apiName =
                std::format("{} {}.{}", GraphicsAPIToString(apiInfo.API), apiInfo.Major, apiInfo.Minor);

            std::string apiText = std::string("Running on : ") + apiName;
            ImGui::Text("%s", apiText.c_str());
            std::string deviceName = std::string("Device: ") + physicalDevice->GetDeviceName();
            ImGui::Text("%s", deviceName.c_str());

            // render framerate
            ImGui::Text("%s", m_FramerateString.c_str());
            m_CurrentDemo->RenderUI();
        }
    }
    else
    {
        RenderDemoList();
    }
}

void DemoImGuiLayer::RenderPerformanceInfo()
{
    NX_PROFILE_FUNCTION();
    if (ImGui::CollapsingHeader("Performance"))
    {
        const auto &results = Nexus::Timings::Profiler::Get().GetResults();
        for (const auto &profileResult : results)
        {
            std::string output = std::string(profileResult.Name) + std::string(": ") +
                                 std::to_string(profileResult.Time.GetMilliseconds<float>()) + std::string(" Ms");
            ImGui::Text("%s", output.c_str());
        }
    }

    Nexus::Timings::Profiler::Get().Reset();
}