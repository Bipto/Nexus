#include "Nexus-Core/Engine.hpp"

namespace Nexus
{
    Engine::Engine(Graphics::GraphicsAPI graphicsAPI, Audio::AudioAPI audioAPI)
    {
        CreateGraphicsResources();
    }

    void Engine::CreateGraphicsResources()
    {
        Nexus::Graphics::GraphicsAPICreateInfo apiCreateInfo = {
            .API = Nexus::Graphics::GraphicsAPI::OpenGL, .Debug = true
        };
        m_GraphicsAPI = std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(
            Nexus::Graphics::IGraphicsAPI::CreateAPI(apiCreateInfo)
        );

        std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>>
            physicalDevices = m_GraphicsAPI->GetPhysicalDevices();
        m_GraphicsDevice = std::unique_ptr<Nexus::Graphics::IGraphicsDevice>(
            m_GraphicsAPI->CreateGraphicsDevice(physicalDevices[0])
        );

        // iterate through all available command queues
        std::vector<Nexus::Graphics::QueueFamilyInfo> queueFamilies =
            m_GraphicsDevice->GetQueueFamilies();
        for (const Nexus::Graphics::QueueFamilyInfo &queueFamily : queueFamilies)
        {
            if (queueFamily.HasCapability(
                    Nexus::Graphics::QueueCapabilities::Graphics
                ) &&
                queueFamily.HasCapability(
                    Nexus::Graphics::QueueCapabilities::Compute
                ) &&
                queueFamily.HasCapability(
                    Nexus::Graphics::QueueCapabilities::Transfer
                ))
            {
                // create graphics queue
                {
                    Nexus::Graphics::CommandQueueDescription queueDesc = {};
                    queueDesc.QueueFamilyIndex = queueFamily.QueueFamily;
                    queueDesc.QueueIndex = 0;
                    queueDesc.DebugName = "Application Graphics Queue";
                    m_GraphicsCommandQueue =
                        m_GraphicsDevice->CreateCommandQueue(queueDesc);
                }
            }
        }
    }

    Nexus::Graphics::IGraphicsAPI *Engine::GetGraphicsAPI()
    {
        return m_GraphicsAPI.get();
    }

    Nexus::Graphics::IGraphicsDevice *Engine::GetGraphicsDevice()
    {
        return m_GraphicsDevice.get();
    }

    Nexus::Graphics::CommandQueueHandle Engine::GetGraphicsCommandQueue()
    {
        return m_GraphicsCommandQueue;
    }
} // namespace Nexus