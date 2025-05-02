#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo info = {};
	info.Debug									= true;
	info.API									= Nexus::Graphics::GraphicsAPI::Vulkan;

	std::unique_ptr<Nexus::Graphics::IGraphicsAPI> api =
		std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(info));

	auto physicalDevices = api->GetPhysicalDevices();
	for (auto physicalDevice : physicalDevices)
	{
		std::cout << physicalDevice->GetDeviceName() << std::endl;
		auto features = physicalDevice->GetPhysicalDeviceFeatures();
	}

	std::unique_ptr<Nexus::Graphics::GraphicsDevice> graphicsDevice =
		std::unique_ptr<Nexus::Graphics::GraphicsDevice>(api->CreateGraphicsDevice(physicalDevices[0]));

	Nexus::Graphics::TextureSpecification textureSpec = {};
	textureSpec.Width								  = 512;
	textureSpec.Height								  = 512;
	textureSpec.ArrayLayers							  = 1;

	std::unique_ptr<Nexus::Graphics::Texture> texture = std::unique_ptr<Nexus::Graphics::Texture>(graphicsDevice->CreateTexture(textureSpec));
}