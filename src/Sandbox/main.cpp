#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo info = {};
	info.Debug									= true;
	info.API									= Nexus::Graphics::GraphicsAPI::OpenGL;

	Nexus::Graphics::IGraphicsAPI *api			   = Nexus::Graphics::IGraphicsAPI::CreateAPI(info);
	auto						   physicalDevices = api->GetPhysicalDevices();
	for (auto physicalDevice : physicalDevices)
	{
		std::cout << physicalDevice->GetDeviceName() << std::endl;
		auto features = physicalDevice->GetPhysicalDeviceFeatures();
	}
	Nexus::Graphics::GraphicsDevice *graphicsDevice = api->CreateGraphicsDevice(physicalDevices[0]);
}