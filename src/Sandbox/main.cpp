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

	{
		glm::vec3 positions[] = {glm::vec3(255.0f, 255.0f, 255.0f), glm::vec3(1, 1, 1), glm::vec3(2, 2, 2)};

		Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
		vertexBufferDesc.Type									  = Nexus::Graphics::DeviceBufferType::Vertex;
		vertexBufferDesc.StrideInBytes							  = sizeof(glm::vec3);
		vertexBufferDesc.SizeInBytes							  = sizeof(positions);
		vertexBufferDesc.HostVisible							  = true;

		std::unique_ptr<Nexus::Graphics::DeviceBuffer> vertexBuffer =
			std::unique_ptr<Nexus::Graphics::DeviceBuffer>(graphicsDevice->CreateDeviceBuffer(vertexBufferDesc));

		vertexBuffer->SetData(positions, 0, sizeof(positions));

		std::vector<char> data = vertexBuffer->GetData(0, sizeof(positions));
	}
}