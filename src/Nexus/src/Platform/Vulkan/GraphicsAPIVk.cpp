#include "GraphicsAPIVk.hpp"
#include "PlatformVk.hpp"

#include "GraphicsDeviceVk.hpp"
#include "PhysicalDeviceVk.hpp"

namespace Nexus::Graphics
{
	std::vector<std::string> GetSupportedInstanceExtensions(const GladVulkanContext &context, std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		uint32_t count	= 0;
		VkResult result = context.EnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

		std::vector<VkExtensionProperties> properties(count);
		result = context.EnumerateInstanceExtensionProperties(nullptr, &count, properties.data());

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to get supported instance extensions");
		}

		std::vector<std::string> extensions;
		for (const auto &property : properties) { extensions.push_back(property.extensionName); }

		return extensions;
	}

	VkResult CreateDebugUtilsMessengerEXT(const GladVulkanContext				   &context,
										  VkInstance								instance,
										  const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
										  const VkAllocationCallbacks			   *pAllocator,
										  VkDebugUtilsMessengerEXT				   *pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)context.GetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(const GladVulkanContext	   &context,
									   VkInstance					instance,
									   VkDebugUtilsMessengerEXT		messenger,
									   const VkAllocationCallbacks *pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)context.GetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, messenger, pAllocator);
		}
	}

	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};

	bool CheckValidationLayerSupport(const GladVulkanContext &context)
	{
		uint32_t layerCount;
		context.EnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		context.EnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char *layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto &layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char *> GetRequiredInstanceExtensions(bool debug)
	{
		std::vector<const char *> extensionNames = PlatformVk::GetRequiredExtensions();

		// required for putting markers in command buffers
		extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		if (debug)
		{
			extensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensionNames.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
		}

		return extensionNames;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		 messageSeverity,
												 VkDebugUtilsMessageTypeFlagsEXT			 messageType,
												 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
												 void										*pUserData)
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	GraphicsAPI_Vk::GraphicsAPI_Vk(const GraphicsAPICreateInfo &createInfo) : m_CreateInfo(createInfo)
	{
		Vk::GladLoaderData emptyLoadData = {};
		gladLoadVulkanContextUserPtr(&m_Context, VK_NULL_HANDLE, (GLADuserptrloadfunc)Vk::GladFunctionLoaderWithInstance, &emptyLoadData);

		if (createInfo.Debug)
		{
			NX_VALIDATE(CheckValidationLayerSupport(m_Context), "Validation layers were requested, but are not available");
		}

		VkApplicationInfo appInfo  = {};
		appInfo.sType			   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName   = "Nexus App";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.pEngineName		   = "Nexus";
		appInfo.engineVersion	   = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.apiVersion		   = VK_API_VERSION_1_0;

		uint32_t apiVersion = 0;
		if (m_Context.EnumerateInstanceVersion != nullptr)
		{
			VkResult result = m_Context.EnumerateInstanceVersion(&apiVersion);
			if (result == VK_SUCCESS)
			{
				appInfo.apiVersion = apiVersion;
			}
		}

		std::vector<VkValidationFeatureEnableEXT> enables = {VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT};

		VkValidationFeaturesEXT validationFeatures		 = {};
		validationFeatures.sType						 = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		validationFeatures.enabledValidationFeatureCount = (uint32_t)enables.size();
		validationFeatures.pEnabledValidationFeatures	 = enables.data();

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType				= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext				= nullptr;

		static bool EnableGPULevelDebugging = false;
		if (createInfo.Debug && EnableGPULevelDebugging)
		{
			instanceCreateInfo.pNext = &validationFeatures;
		}

		instanceCreateInfo.pApplicationInfo = &appInfo;

		if (createInfo.Debug)
		{
			instanceCreateInfo.enabledLayerCount   = validationLayers.size();
			instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
		}

		auto extensions							   = GetRequiredInstanceExtensions(createInfo.Debug);
		instanceCreateInfo.enabledExtensionCount   = (uint32_t)extensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		NX_VALIDATE(m_Context.CreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create Vulkan instance");

		Vk::GladLoaderData loadData = {.instance = m_Instance, .device = VK_NULL_HANDLE};
		gladLoadVulkanContextUserPtr(&m_Context, VK_NULL_HANDLE, (GLADuserptrloadfunc)Vk::GladFunctionLoaderWithInstance, &loadData);

		if (createInfo.Debug)
		{
			SetupDebugMessenger();
		}
	}

	GraphicsAPI_Vk::~GraphicsAPI_Vk()
	{
		if (m_CreateInfo.Debug)
		{
			DestroyDebugUtilsMessengerEXT(m_Context, m_Instance, m_DebugMessenger, nullptr);
		}

		m_Context.DestroyInstance(m_Instance, nullptr);
	}

	std::vector<std::shared_ptr<IPhysicalDevice>> GraphicsAPI_Vk::GetPhysicalDevices()
	{
		std::vector<std::shared_ptr<IPhysicalDevice>> physicalDevices;

		uint32_t physicalDeviceCount = 0;
		m_Context.EnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> vulkanPhysicalDevices(physicalDeviceCount);
		m_Context.EnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, vulkanPhysicalDevices.data());

		for (VkPhysicalDevice vulkanPhysicalDevice : vulkanPhysicalDevices)
		{
			physicalDevices.push_back(std::make_shared<Graphics::PhysicalDeviceVk>(m_Context, vulkanPhysicalDevice));
		}

		return physicalDevices;
	}

	Graphics::GraphicsDevice *GraphicsAPI_Vk::CreateGraphicsDevice(std::shared_ptr<IPhysicalDevice> device)
	{
		VulkanDeviceConfig config			  = {};
		config.Debug						  = m_CreateInfo.Debug;
		config.UseDynamicRenderingIfAvailable = true;
		return new GraphicsDeviceVk(device, m_Instance, config);
	}

	const GraphicsAPICreateInfo &GraphicsAPI_Vk::GetGraphicsAPICreateInfo() const
	{
		return m_CreateInfo;
	}

	void GraphicsAPI_Vk::SetupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType							  = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData	   = nullptr;

		NX_VALIDATE(CreateDebugUtilsMessengerEXT(m_Context, m_Instance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS,
					"Failed to create Debug Messenger");
	}
}	 // namespace Nexus::Graphics
