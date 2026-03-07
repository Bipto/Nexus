#pragma once

#include "RHI/CommandList.hpp"
#include "RHI/Fence.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/Swapchain.hpp"
#include "RHI/Texture.hpp"
#include "RHI/Utils.hpp"
#include <RHI/Types.hpp>

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

	class NX_RHI_API ICommandQueue
	{
	  public:
		virtual ~ICommandQueue()																							 = default;
		virtual const CommandQueueDescription &GetDescription() const														 = 0;
		virtual Ref<ISwapchain>				   CreateSwapchain(const SwapchainDescription &spec)							 = 0;
		virtual void						   SubmitCommandList(Ref<ICommandList> commandList)								 = 0;
		virtual void						   SubmitCommandList(Ref<ICommandList> commandList, Ref<IFence> fence)			 = 0;
		virtual void						   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists) = 0;
		virtual void						   SubmitCommandLists(Ref<ICommandList> *commandLists, uint32_t numCommandLists, Ref<IFence> fence) = 0;
		virtual IGraphicsDevice				  *GetGraphicsDevice()																				= 0;
		virtual bool						   WaitForIdle()																					= 0;

		/// @brief A pure virtual method that creates a new command list
		/// @return A pointer to a command list
		virtual Ref<ICommandList> CreateCommandList(const CommandListDescription &spec = {}) = 0;
	};
}	 // namespace Nexus::Graphics