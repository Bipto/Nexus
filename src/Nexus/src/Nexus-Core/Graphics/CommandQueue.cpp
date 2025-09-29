#include "Nexus-Core/Graphics/CommandQueue.hpp"
#include <Nexus-Core/Graphics/CommandList.hpp>
#include <Nexus-Core/Graphics/DeviceBuffer.hpp>
#include <Nexus-Core/Graphics/Fence.hpp>
#include <Nexus-Core/Graphics/PixelFormat.hpp>
#include <Nexus-Core/Graphics/Texture.hpp>
#include <Nexus-Core/Types.hpp>
#include <Nexus-Core/Utils/Utils.hpp>

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	void ICommandQueue::SubmitCommandList(Ref<CommandList> commandList)
	{
		SubmitCommandList(commandList, nullptr);
	}

	void ICommandQueue::SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence)
	{
		SubmitCommandLists(&commandList, 1, fence);
	}

	void ICommandQueue::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists)
	{
		SubmitCommandLists(commandLists, numCommandLists, nullptr);
	}

	void ICommandQueue::WriteToTexture(Ref<Texture> texture,
									   uint32_t		arrayLayer,
									   uint32_t		mipLevel,
									   uint32_t		x,
									   uint32_t		y,
									   uint32_t		z,
									   uint32_t		width,
									   uint32_t		height,
									   const void  *data,
									   size_t		size)
	{
		GraphicsDevice *device = GetGraphicsDevice();

		DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access				   = BufferMemoryAccess::Upload;
		bufferDesc.Usage				   = BUFFER_USAGE_NONE;
		bufferDesc.SizeInBytes			   = size;
		bufferDesc.StrideInBytes		   = size;
		Ref<DeviceBuffer> buffer		   = device->CreateDeviceBuffer(bufferDesc);
		Ref<CommandList>  cmdList		   = CreateCommandList();

		buffer->SetData(data, 0, size);

		cmdList->Begin();

		BufferTextureCopyDescription copyDesc = {};
		copyDesc.BufferHandle				  = buffer;
		copyDesc.BufferOffset				  = 0;
		copyDesc.BufferRowLength			  = 0;
		copyDesc.BufferImageHeight			  = 0;
		copyDesc.TextureHandle				  = texture;
		copyDesc.TextureSubresource			  = {.MipLevel = mipLevel, .BaseArrayLayer = arrayLayer, .LayerCount = 1};
		copyDesc.TextureOffset				  = {.X = (int32_t)x, .Y = (int32_t)y, .Z = (int32_t)z};
		copyDesc.TextureExtent				  = {.Width = width, .Height = height, .Depth = 1};
		cmdList->CopyBufferToTexture(copyDesc);

		cmdList->End();
		SubmitCommandLists(&cmdList, 1, nullptr);
		WaitForIdle();
	}

	std::vector<char> ICommandQueue::ReadFromTexture(Ref<Texture> texture,
													 uint32_t	  arrayLayer,
													 uint32_t	  mipLevel,
													 uint32_t	  x,
													 uint32_t	  y,
													 uint32_t	  z,
													 uint32_t	  width,
													 uint32_t	  height)
	{
		GraphicsDevice *device	   = GetGraphicsDevice();
		size_t			bufferSize = width * height * GetPixelFormatSizeInBytes(texture->GetDescription().Format);

		DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access				   = BufferMemoryAccess::Readback;
		bufferDesc.Usage				   = BUFFER_USAGE_NONE;
		bufferDesc.SizeInBytes			   = bufferSize;
		bufferDesc.StrideInBytes		   = bufferSize;

		Ref<DeviceBuffer> buffer  = device->CreateDeviceBuffer(bufferDesc);
		Ref<CommandList>  cmdList = CreateCommandList();

		cmdList->Begin();

		BufferTextureCopyDescription copyDesc = {};
		copyDesc.BufferHandle				  = buffer;
		copyDesc.BufferOffset				  = 0;
		copyDesc.BufferRowLength			  = 0;
		copyDesc.BufferImageHeight			  = 0;
		copyDesc.TextureHandle				  = texture;
		copyDesc.TextureOffset				  = {.X = (int32_t)x, .Y = (int32_t)y, .Z = (int32_t)z};
		copyDesc.TextureExtent				  = {.Width = width, .Height = height, .Depth = 1};
		copyDesc.TextureSubresource			  = {.MipLevel = mipLevel, .BaseArrayLayer = arrayLayer, .LayerCount = 1};

		cmdList->CopyTextureToBuffer(copyDesc);

		cmdList->End();
		SubmitCommandLists(&cmdList, 1, nullptr);
		WaitForIdle();

		return buffer->GetData(0, bufferSize);
	}
}	 // namespace Nexus::Graphics