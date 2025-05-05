#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

int main()
{
	Nexus::Graphics::GraphicsAPICreateInfo info = {};
	info.Debug									= true;
	info.API									= Nexus::Graphics::GraphicsAPI::OpenGL;

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

	Nexus::Ref<Nexus::Graphics::CommandList> cmdList = graphicsDevice->CreateCommandList();

	Nexus::Graphics::DeviceBufferDescription bufferDesc = {};
	bufferDesc.Type										= Nexus::Graphics::DeviceBufferType::Upload;
	bufferDesc.SizeInBytes								= sizeof(uint32_t);
	bufferDesc.StrideInBytes							= sizeof(uint32_t);
	bufferDesc.HostVisible								= true;

	std::unique_ptr<Nexus::Graphics::DeviceBuffer> uploadBuffer =
		std::unique_ptr<Nexus::Graphics::DeviceBuffer>(graphicsDevice->CreateDeviceBuffer(bufferDesc));
	std::unique_ptr<Nexus::Graphics::DeviceBuffer> readbackBuffer =
		std::unique_ptr<Nexus::Graphics::DeviceBuffer>(graphicsDevice->CreateDeviceBuffer(bufferDesc));

	Nexus::Graphics::TextureSpecification textureSpec = {};
	textureSpec.Width								  = 1;
	textureSpec.Height								  = 1;
	textureSpec.ArrayLayers							  = 1;
	textureSpec.MipLevels							  = 1;

	std::unique_ptr<Nexus::Graphics::Texture> sourceTexture = std::unique_ptr<Nexus::Graphics::Texture>(graphicsDevice->CreateTexture(textureSpec));
	std::unique_ptr<Nexus::Graphics::Texture> destTexture	= std::unique_ptr<Nexus::Graphics::Texture>(graphicsDevice->CreateTexture(textureSpec));

	uint32_t col = 0xFF0000FF;
	uploadBuffer->SetData(&col, 0, sizeof(col));

	cmdList->Begin();

	Nexus::Graphics::BufferTextureCopyDescription uploadCopyDesc = {};
	uploadCopyDesc.BufferHandle									 = uploadBuffer.get();
	uploadCopyDesc.TextureHandle								 = sourceTexture.get();
	uploadCopyDesc.X											 = 0;
	uploadCopyDesc.Y											 = 0;
	uploadCopyDesc.Z											 = 0;
	uploadCopyDesc.Width										 = 1;
	uploadCopyDesc.Height										 = 1;
	uploadCopyDesc.Depth										 = 1;
	uploadCopyDesc.BufferOffset									 = 0;
	uploadCopyDesc.Aspect										 = Nexus::Graphics::ImageAspect::Colour;
	cmdList->CopyBufferToTexture(uploadCopyDesc);

	Nexus::Graphics::TextureCopyDescription textureCopyDesc = {};
	textureCopyDesc.Source									= sourceTexture.get();
	textureCopyDesc.Destination								= destTexture.get();
	textureCopyDesc.X										= 0;
	textureCopyDesc.Y										= 0;
	textureCopyDesc.Z										= 0;
	textureCopyDesc.Width									= 1;
	textureCopyDesc.Height									= 1;
	textureCopyDesc.Depth									= 1;
	textureCopyDesc.MipLevel								= 0;
	textureCopyDesc.Aspect									= Nexus::Graphics::ImageAspect::Colour;
	cmdList->CopyTextureToTexture(textureCopyDesc);

	Nexus::Graphics::BufferTextureCopyDescription readbackCopyDesc = {};
	readbackCopyDesc.BufferHandle								   = readbackBuffer.get();
	readbackCopyDesc.TextureHandle								   = destTexture.get();
	readbackCopyDesc.X											   = 0;
	readbackCopyDesc.Y											   = 0;
	readbackCopyDesc.Z											   = 0;
	readbackCopyDesc.Width										   = 1;
	readbackCopyDesc.Height										   = 1;
	readbackCopyDesc.Depth										   = 1;
	readbackCopyDesc.BufferOffset								   = 0;
	readbackCopyDesc.Aspect										   = Nexus::Graphics::ImageAspect::Colour;
	cmdList->CopyTextureToBuffer(readbackCopyDesc);

	/* Nexus::Graphics::BufferCopyDescription bufferCopyDesc = {};
	bufferCopyDesc.Source								  = uploadBuffer.get();
	bufferCopyDesc.Destination							  = readbackBuffer.get();
	bufferCopyDesc.ReadOffset							  = 0;
	bufferCopyDesc.WriteOffset							  = 0;
	bufferCopyDesc.Size									  = sizeof(uint32_t);
	cmdList->CopyBufferToBuffer(bufferCopyDesc); */

	cmdList->End();

	graphicsDevice->SubmitCommandList(cmdList);

	std::vector<char> pixel = readbackBuffer->GetData(0, sizeof(uint32_t));
}