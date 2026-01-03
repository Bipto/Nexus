#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

#include "Nexus-Core/Renderer/BatchRenderer.hpp"

#include "Audio/AudioDevice.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo createInfo = {};
	createInfo.API									  = Nexus::Graphics::GraphicsAPI::OpenGL;
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI> api =
		std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(createInfo));

	std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>> physicalDevices = api->GetPhysicalDevices();
	std::unique_ptr<Nexus::Graphics::IGraphicsDevice>			   device =
		std::unique_ptr<Nexus::Graphics::IGraphicsDevice>(api->CreateGraphicsDevice(physicalDevices[0]));
}