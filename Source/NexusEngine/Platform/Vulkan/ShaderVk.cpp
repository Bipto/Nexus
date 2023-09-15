#if defined(NX_PLATFORM_VULKAN)

#include "ShaderVk.hpp"

namespace Nexus::Graphics
{
    ShaderVk::ShaderVk(const std::vector<uint32_t> &vertexBytecode, std::vector<uint32_t> &fragmentBytecode, const std::string &vertexSource, const std::string &fragmentSource, const VertexBufferLayout &layout, GraphicsDeviceVk *graphicsDevice)
        : m_VertexShaderSource(vertexSource), m_FragmentShaderSource(fragmentSource), m_Layout(layout), m_GraphicsDevice(graphicsDevice)
    {
        bool success;
        m_VertexShader = CreateShaderModule(vertexBytecode, &success);
        if (!success)
        {
            throw std::runtime_error("Failed to create vertex shader module");
        }

        m_FragmentShader = CreateShaderModule(fragmentBytecode, &success);
        if (!success)
        {
            throw std::runtime_error("Failed to create fragment shader module");
        }

        VkPipelineShaderStageCreateInfo vertexInfo = {};
        vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexInfo.pNext = nullptr;
        vertexInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexInfo.module = m_VertexShader;
        vertexInfo.pName = "main";
        m_ShaderStages.push_back(vertexInfo);

        VkPipelineShaderStageCreateInfo fragmentInfo = {};
        fragmentInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentInfo.pNext = nullptr;
        fragmentInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentInfo.module = m_FragmentShader;
        fragmentInfo.pName = "main";
        m_ShaderStages.push_back(fragmentInfo);
    }

    const std::string &ShaderVk::GetVertexShaderSource()
    {
        return m_VertexShaderSource;
    }

    const std::string &ShaderVk::GetFragmentShaderSource()
    {
        return m_FragmentShaderSource;
    }

    const VertexBufferLayout &ShaderVk::GetLayout() const
    {
        return m_Layout;
    }

    const std::vector<VkPipelineShaderStageCreateInfo> &ShaderVk::GetShaderStages()
    {
        return m_ShaderStages;
    }

    VkShaderModule ShaderVk::CreateShaderModule(const std::vector<uint32_t> &spirv_buffer, bool *successful)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.codeSize = spirv_buffer.size() * sizeof(uint32_t);
        createInfo.pCode = spirv_buffer.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_GraphicsDevice->GetVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            *successful = false;
        }

        *successful = true;

        return shaderModule;
    }
}

#endif