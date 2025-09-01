#pragma once

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/Fence.hpp"
#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Utils/Utils.hpp"
#include "Texture.hpp"
#include <Nexus-Core/Types.hpp>

namespace Nexus::Graphics
{
	enum QueueCapabilities
	{
		Invalid	 = 0,
		Graphics = BIT(0),
		Compute	 = BIT(2),
		Transfer = BIT(3),
		All		 = Graphics | Compute | Transfer
	};

	struct QueueFamilyInfo
	{
		QueueCapabilities Capabilities;
		uint32_t		  QueueFamily = 0;
		uint32_t		  QueueCount  = 0;
	};

	struct CommandQueueDescription
	{
		uint32_t	QueueFamilyIndex = 0;
		uint32_t	QueueIndex		 = 0;
		std::string DebugName		 = "Queue";
	};

	class GraphicsDevice;

	class NX_API ICommandQueue
	{
	  public:
		virtual ~ICommandQueue() = default;
		void					SubmitCommandList(Ref<CommandList> commandList);
		void					SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence);
		void					SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists);
		virtual void			SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) = 0;
		virtual void			Present(Ref<Swapchain> swapchain)															   = 0;
		virtual GraphicsDevice *GetGraphicsDevice()																			   = 0;

		void WriteToTexture(Ref<Texture> texture,
							uint32_t	 arrayLayer,
							uint32_t	 mipLevel,
							uint32_t	 x,
							uint32_t	 y,
							uint32_t	 z,
							uint32_t	 width,
							uint32_t	 height,
							const void	*data,
							size_t		 size);

		std::vector<char> ReadFromTexture(Ref<Texture> texture,
										  uint32_t	   arrayLayer,
										  uint32_t	   mipLevel,
										  uint32_t	   x,
										  uint32_t	   y,
										  uint32_t	   z,
										  uint32_t	   width,
										  uint32_t	   height);
	};
}	 // namespace Nexus::Graphics