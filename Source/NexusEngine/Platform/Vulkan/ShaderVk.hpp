#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus/Graphics/Shader.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class ShaderVk : public Shader
    {
    public:
        ShaderVk(const std::vector<uint32_t> &vertexBytecode, std::vector<uint32_t> &fragmentBytecode, const std::string &vertexSource, const std::string &fragmentSource, GraphicsDeviceVk *graphicsDevice);
        ~ShaderVk();

        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;
        const std::vector<VkPipelineShaderStageCreateInfo> &GetShaderStages();

    private:
        VkShaderModule CreateShaderModule(const std::vector<uint32_t> &spirv_buffer, bool *successful);

    private:
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;

        VkShaderModule m_VertexShader;
        VkShaderModule m_FragmentShader;

        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

        GraphicsDeviceVk *m_GraphicsDevice;
    };
}

#endif