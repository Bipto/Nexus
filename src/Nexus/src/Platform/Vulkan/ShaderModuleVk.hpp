#pragma once

#include "Nexus-Core/Graphics/ShaderModule.hpp"
#include "Vk.hpp"

#if defined(NX_PLATFORM_VULKAN)

namespace Nexus::Graphics
{
	class GraphicsDeviceVk;

	class ShaderModuleVk : public ShaderModule
	{
	  public:
		ShaderModuleVk(const ShaderModuleSpecification &shaderModuleSpec, GraphicsDeviceVk *device);
		virtual ~ShaderModuleVk();
		VkShaderModule GetShaderModule();
		ShaderReflectionData Reflect() const final;

	  private:
		void CreateShaderModule();

	  private:
		GraphicsDeviceVk *m_GraphicsDevice = nullptr;
		VkShaderModule	  m_ShaderModule;
	};
}	 // namespace Nexus::Graphics

#endif