#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "CommandExecutorOpenGL.hpp"
#include "FramebufferOpenGL.hpp"
#include "GL.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "SwapchainOpenGL.hpp"

namespace Nexus::Graphics
{
class GraphicsDeviceOpenGL : public GraphicsDevice
{
  public:
    GraphicsDeviceOpenGL(const GraphicsDeviceSpecification &createInfo, Window *window, const SwapchainSpecification &swapchainSpec);
    GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL &) = delete;

    void SetFramebuffer(Ref<Framebuffer> framebuffer);
    void SetSwapchain(Swapchain *swapchain);
    virtual void SubmitCommandList(Ref<CommandList> commandList) override;

    virtual const std::string GetAPIName() override;
    virtual const char *GetDeviceName() override;

    virtual Ref<Texture> CreateTexture(const TextureSpecification &spec) override;
    virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
    virtual Ref<CommandList> CreateCommandList(const CommandListSpecification &spec = {});
    virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data) override;
    virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) override;
    virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;
    virtual Ref<ResourceSet> CreateResourceSet(const ResourceSetSpecification &spec) override;
    virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification &spec) override;
    virtual Ref<Sampler> CreateSampler(const SamplerSpecification &spec) override;
    virtual Ref<TimingQuery> CreateTimingQuery() override;

    virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;

    virtual ShaderLanguage GetSupportedShaderFormat() override;
    virtual float GetUVCorrection()
    {
        return 1.0f;
    }
    virtual bool IsUVOriginTopLeft() override
    {
        return false;
    };

    void OpenGLCall(std::function<void()> func);

  private:
    virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;
    std::vector<std::string> GetSupportedExtensions();

  private:
    const char *m_GlslVersion;
    WeakRef<FramebufferOpenGL> m_BoundFramebuffer = {};
    VSyncState m_VsyncState = VSyncState::Enabled;

    std::vector<std::string> m_Extensions{};

    CommandExecutorOpenGL m_CommandExecutor{};
};
} // namespace Nexus::Graphics

#endif