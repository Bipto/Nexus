#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

#include "Nexus-Core/Renderer/BatchRenderer.hpp"

#include "Nexus-Core/Audio/AudioDevice.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo createInfo = {};
	createInfo.API									  = Nexus::Graphics::GraphicsAPI::OpenGL;
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI> api =
		std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(createInfo));

	std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>> physicalDevices = api->GetPhysicalDevices();
	std::unique_ptr<Nexus::Graphics::GraphicsDevice>			   device =
		std::unique_ptr<Nexus::Graphics::GraphicsDevice>(api->CreateGraphicsDevice(physicalDevices[0]));

	Nexus::Graphics::FramebufferSpecification framebufferSpec = {};
	framebufferSpec.Width									  = 512;
	framebufferSpec.Height									  = 512;
	framebufferSpec.ColorAttachmentSpecification			  = {{Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm}};

	Nexus::Ref<Nexus::Graphics::Framebuffer> framebuffer = device->CreateFramebuffer(framebufferSpec);
}