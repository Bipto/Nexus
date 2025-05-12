#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo createInfo = {};
	createInfo.API									  = Nexus::Graphics::GraphicsAPI::OpenGL;
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI> api =
		std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(createInfo));

	std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>> physicalDevices = api->GetPhysicalDevices();
	std::unique_ptr<Nexus::Graphics::GraphicsDevice>			   device =
		std::unique_ptr<Nexus::Graphics::GraphicsDevice>(api->CreateGraphicsDevice(physicalDevices[0]));

	Nexus::Graphics::FenceDescription fenceDesc	  = {};
	fenceDesc.Signalled							  = true;
	std::unique_ptr<Nexus::Graphics::Fence> fence = std::unique_ptr<Nexus::Graphics::Fence>(device->CreateFence(fenceDesc));

	std::array<Nexus::Graphics::Fence *, 1> fences = {fence.get()};
	Nexus::Graphics::FenceWaitResult result = device->WaitForFences(fences.data(), fences.size(), true, Nexus::TimeSpan::FromNanoseconds(UINT64_MAX));
	device->ResetFences(fences.data(), fences.size());
	bool signalled = fence->IsSignalled();
}