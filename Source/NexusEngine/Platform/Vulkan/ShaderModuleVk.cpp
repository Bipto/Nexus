#if defined(NX_PLATFORM_VULKAN)

#include "ShaderModuleVk.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    ShaderModuleVk::ShaderModuleVk(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec, GraphicsDeviceVk *device)
        : ShaderModule(shaderModuleSpec, resourceSpec), m_GraphicsDevice(device)
    {
        CreateShaderModule();
    }

    VkShaderModule ShaderModuleVk::GetShaderModule()
    {
        return m_ShaderModule;
    }

    void ShaderModuleVk::CreateShaderModule()
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.codeSize = m_ModuleSpecification.SpirvBinary.size() * sizeof(uint32_t);
        createInfo.pCode = m_ModuleSpecification.SpirvBinary.data();

        if (vkCreateShaderModule(m_GraphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module");
        }
    }
}

#endif