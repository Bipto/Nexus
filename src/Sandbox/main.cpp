#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo info = {};
	info.Debug									= true;
	info.API									= Nexus::Graphics::GraphicsAPI::D3D12;

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

		Nexus::Graphics::DeviceBufferDescription uploadBufferDesc = {};
		uploadBufferDesc.Type									  = Nexus::Graphics::DeviceBufferType::Upload;
		uploadBufferDesc.StrideInBytes							  = sizeof(glm::vec3);
		uploadBufferDesc.SizeInBytes							  = sizeof(positions);

		std::unique_ptr<Nexus::Graphics::DeviceBuffer> uploadBuffer =
			std::unique_ptr<Nexus::Graphics::DeviceBuffer>(graphicsDevice->CreateDeviceBuffer(uploadBufferDesc));

		uploadBuffer->SetData(positions, 0, sizeof(positions));

		Nexus::Graphics::DeviceBufferDescription vertexBufferDesc = {};
		vertexBufferDesc.Type									  = Nexus::Graphics::DeviceBufferType::Vertex;
		vertexBufferDesc.StrideInBytes							  = sizeof(glm::vec3);
		vertexBufferDesc.SizeInBytes							  = sizeof(positions);

		std::unique_ptr<Nexus::Graphics::DeviceBuffer> vertexBuffer =
			std::unique_ptr<Nexus::Graphics::DeviceBuffer>(graphicsDevice->CreateDeviceBuffer(vertexBufferDesc));

		Nexus::Graphics::BufferCopyDescription uploadBufferCopyDesc = {};
		uploadBufferCopyDesc.Source									= uploadBuffer.get();
		uploadBufferCopyDesc.Target									= vertexBuffer.get();
		uploadBufferCopyDesc.Size									= sizeof(positions);
		uploadBufferCopyDesc.ReadOffset								= 0;
		uploadBufferCopyDesc.WriteOffset							= 0;
		graphicsDevice->CopyBuffer(uploadBufferCopyDesc);

		Nexus::Graphics::DeviceBufferDescription readbackBufferDesc = {};
		readbackBufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Readback;
		readbackBufferDesc.StrideInBytes							= sizeof(glm::vec3);
		readbackBufferDesc.SizeInBytes								= sizeof(positions);

		std::unique_ptr<Nexus::Graphics::DeviceBuffer> readbackBuffer =
			std::unique_ptr<Nexus::Graphics::DeviceBuffer>(graphicsDevice->CreateDeviceBuffer(readbackBufferDesc));

		Nexus::Graphics::BufferCopyDescription readbackBufferCopyDesc = {};
		readbackBufferCopyDesc.Source								  = vertexBuffer.get();
		readbackBufferCopyDesc.Target								  = readbackBuffer.get();
		readbackBufferCopyDesc.Size									  = sizeof(positions);
		readbackBufferCopyDesc.ReadOffset							  = 0;
		readbackBufferCopyDesc.WriteOffset							  = 0;
		graphicsDevice->CopyBuffer(readbackBufferCopyDesc);

		std::vector<char> data = readbackBuffer->GetData(0, sizeof(positions));
	}
}