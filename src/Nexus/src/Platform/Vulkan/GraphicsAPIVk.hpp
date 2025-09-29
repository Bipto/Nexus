#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class GraphicsAPI_Vk : public IGraphicsAPI
	{
	  public:
		GraphicsAPI_Vk(const GraphicsAPICreateInfo &createInfo);
		virtual ~GraphicsAPI_Vk();
		virtual std::vector<std::shared_ptr<IPhysicalDevice>> GetPhysicalDevices() override;
		virtual Graphics::GraphicsDevice					 *CreateGraphicsDevice(std::shared_ptr<IPhysicalDevice> device) override;
		virtual const GraphicsAPICreateInfo					 &GetGraphicsAPICreateInfo() const override;

	  private:
		void SetupDebugMessenger();

	  private:
		GraphicsAPICreateInfo	 m_CreateInfo	  = {};
		VkInstance				 m_Instance		  = {};
		VkDebugUtilsMessengerEXT m_DebugMessenger = {};
		GladVulkanContext		 m_Context		  = {};
	};
}	 // namespace Nexus::Graphics

#endif