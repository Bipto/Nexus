#pragma once

#include "CommandExecutorSoftware.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceSoftware : public GraphicsDevice
	{
	  public:
		GraphicsDeviceSoftware(const GraphicsDeviceSpecification &createInfo, Window *window, const SwapchainSpecification &swapchainSpec);
		GraphicsDeviceSoftware(const GraphicsDeviceSoftware &) = delete;
		virtual ~GraphicsDeviceSoftware();

		virtual const std::string GetAPIName() override;
		virtual const char		 *GetDeviceName() override;
		virtual void			  SubmitCommandList(Ref<CommandList> commandList) override;

		virtual Ref<Pipeline>	   CreatePipeline(const PipelineDescription &description) override;
		virtual Ref<VertexBuffer>  CreateVertexBuffer(const BufferDescription &description, const void *data) override;
		virtual Ref<IndexBuffer>   CreateIndexBuffer(const BufferDescription &description,
													 const void				 *data,
													 IndexBufferFormat		  format = IndexBufferFormat::UInt32) override;
		virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;
		virtual Ref<CommandList>   CreateCommandList(const CommandListSpecification &spec = {}) override;
		virtual Ref<Texture2D>	   CreateTexture2D(const Texture2DSpecification &spec) override;
		virtual Ref<Cubemap>	   CreateCubemap(const CubemapSpecification &spec) override;
		virtual Ref<Framebuffer>   CreateFramebuffer(const FramebufferSpecification &spec) override;
		virtual Ref<ResourceSet>   CreateResourceSet(const ResourceSetSpecification &spec) override;
		Ref<ResourceSet>		   CreateResourceSet(Ref<Pipeline> pipeline);
		virtual Ref<Sampler>	   CreateSampler(const SamplerSpecification &spec) override;
		virtual Ref<TimingQuery>   CreateTimingQuery() override;
		virtual ShaderLanguage	   GetSupportedShaderFormat() override;

		virtual float					   GetUVCorrection() override;
		virtual bool					   IsUVOriginTopLeft() override;
		virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;

	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;

	  private:
		CommandExecutorSoftware m_CommandExecutor = {};
	};
}	 // namespace Nexus::Graphics