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
		Invalid		  = 0,
		Graphics	  = BIT(0),
		Compute		  = BIT(2),
		Transfer	  = BIT(3),
		SparseBinding = BIT(4),
		VideoEncode	  = BIT(5),
		VideoDecode	  = BIT(6)
	};

	struct QueueFamilyInfo
	{
		QueueCapabilities Capabilities;
		uint32_t		  QueueFamily = 0;
		uint32_t		  QueueCount  = 0;

		bool HasCapability(QueueCapabilities caps) const
		{
			return Capabilities & caps;
		}
	};

	struct CommandQueueDescription
	{
		uint32_t	QueueFamilyIndex = 0;
		uint32_t	QueueIndex		 = 0;
		std::string DebugName		 = "Queue";
	};

	class IGraphicsDevice;

	class NX_API ICommandQueue
	{
	  public:
		virtual ~ICommandQueue()																				  = default;
		virtual const CommandQueueDescription &GetDescription() const											  = 0;
		virtual Ref<ISwapchain>				   CreateSwapchain(IWindow *window, const SwapchainDescription &spec) = 0;
		void								   SubmitCommandList(Ref<ICommandList> commandList);
		void								   SubmitCommandList(Ref<ICommandList> commandList, Ref<IFence> fence);
		void								   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists);
		virtual void						   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists, Ref<IFence> fence) = 0;
		virtual IGraphicsDevice				  *GetGraphicsDevice()																				= 0;
		virtual bool						   WaitForIdle()																					= 0;

		/// @brief A pure virtual method that creates a new command list
		/// @return A pointer to a command list
		virtual Ref<ICommandList> CreateCommandList(const CommandListDescription &spec = {}) = 0;

		void WriteToTexture(Ref<ITexture> texture,
							uint32_t	  mipLevel,
							uint32_t	  x,
							uint32_t	  y,
							uint32_t	  z,
							uint32_t	  width,
							uint32_t	  height,
							const void	 *data,
							size_t		  size);

		std::vector<char> ReadFromTexture(Ref<ITexture> texture,
										  uint32_t		mipLevel,
										  uint32_t		x,
										  uint32_t		y,
										  uint32_t		z,
										  uint32_t		width,
										  uint32_t		height);

		void WriteToBuffer(Ref<IDeviceBuffer> buffer, const void *data, size_t offset, size_t size);

		std::vector<char> ReadFromBuffer(Ref<IDeviceBuffer> buffer, size_t offset);
	};
}	 // namespace Nexus::Graphics