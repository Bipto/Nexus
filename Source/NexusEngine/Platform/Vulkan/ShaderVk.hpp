#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Core/Graphics/Shader.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class ShaderVk : public Shader
    {
    public:
        ShaderVk(const std::vector<uint32_t> &vertexBytecode, std::vector<uint32_t> &fragmentBytecode, const std::string &vertexSource, const std::string &fragmentSource, const VertexBufferLayout &layout, GraphicsDeviceVk *graphicsDevice);
        virtual void SetTexture(Ref<Texture> texture, const TextureResourceBinding &binding) override;
        virtual const std::string &GetVertexShaderSource() override;
        virtual const std::string &GetFragmentShaderSource() override;
        virtual const VertexBufferLayout &GetLayout() const override;
        virtual void BindUniformBuffer(Ref<UniformBuffer> buffer, const UniformResourceBinding &binding) override;
        const std::vector<VkPipelineShaderStageCreateInfo> &GetShaderStages();

    private:
        VkShaderModule CreateShaderModule(const std::vector<uint32_t> &spirv_buffer, bool *successful);

    private:
        std::string m_VertexShaderSource;
        std::string m_FragmentShaderSource;
        VertexBufferLayout m_Layout;

        VkShaderModule m_VertexShader;
        VkShaderModule m_FragmentShader;

        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

        GraphicsDeviceVk *m_GraphicsDevice;
    };
}

#endif