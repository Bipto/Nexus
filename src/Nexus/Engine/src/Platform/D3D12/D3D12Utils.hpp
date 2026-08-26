#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "Nexus-Core/ApplicationDescription.hpp"
#include "Nexus-Core/Vertex.hpp"
#include "RHI/CommandList.hpp"
#include "RHI/DeviceBuffer.hpp"
#include "RHI/PixelFormat.hpp"
#include "RHI/SamplerState.hpp"
#include "RHI/ShaderModule.hpp"
#include "RHI/ShaderResources.hpp"
#include "RHI/Texture.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceD3D12;
}

namespace Nexus::D3D12
{
    enum class DescriptorHandleSource
    {
        SRV_UAV_CBV,
        ISampler
    };

    struct DescriptorTableInfo
    {
        size_t Offset = 0;
        DescriptorHandleSource Source = DescriptorHandleSource::SRV_UAV_CBV;
    };

    struct DescriptorHandleInfo
    {
        std::map<std::string, std::vector<uint32_t>> SamplerIndexes = {};
        std::map<std::string, std::vector<uint32_t>> NonSamplerIndexes = {};
        std::vector<DescriptorTableInfo> DescriptorTables = {};
        uint32_t SamplerHeapCount = 0;
        uint32_t SRV_UAV_CBV_HeapCount = 0;
        std::map<std::string, std::string> CombinedImageSamplerMap = {};
        std::map<std::string, Graphics::StorageResourceAccess> StorageBuffers = {};
    };

    DXGI_FORMAT
    GetD3D12PixelFormat(Nexus::Graphics::PixelFormat format);
    D3D12_TEXTURE_LAYOUT GetTextureLayout(Graphics::TextureTiling tiling);

    DXGI_FORMAT
    GetD3D12BaseType(const Nexus::Graphics::VertexBufferElement &element);
    D3D12_CULL_MODE GetCullMode(Nexus::Graphics::CullMode cullMode);
    D3D12_COMPARISON_FUNC
    GetComparisonFunction(Nexus::Graphics::ComparisonFunction function);
    D3D12_STENCIL_OP
    GetStencilOperation(Nexus::Graphics::StencilOperation operation);
    D3D12_FILL_MODE GetFillMode(Nexus::Graphics::FillMode fillMode);
    D3D12_BLEND GetBlendFunction(Nexus::Graphics::BlendFactor function);
    D3D12_BLEND_OP GetBlendEquation(Nexus::Graphics::BlendEquation equation);

    D3D12_FILTER GetD3D12Filter(Nexus::Graphics::SamplerFilter filter);
    D3D12_TEXTURE_ADDRESS_MODE
    GetD3D12TextureAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

    DXGI_FORMAT
    GetD3D12IndexBufferFormat(Nexus::Graphics::IndexFormat format);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE
    GetPipelineTopology(Nexus::Graphics::Topology topology);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::GraphicsPipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipeline(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::ComputePipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateMeshletPipeline(
        Graphics::GraphicsDeviceD3D12 *device, const Graphics::MeshletPipelineDescription &description,
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

    D3D12_HEAP_TYPE GetHeapType(const Graphics::DeviceBufferDescription &desc);
    D3D12_RESOURCE_DIMENSION GetResourceDimensions(Nexus::Graphics::TextureType textureType);
    D3D12_RESOURCE_FLAGS GetResourceFlags(const Graphics::TextureDescription &description);

    D3D12_SHADER_RESOURCE_VIEW_DESC CreateTextureSrvView(const Graphics::TextureViewDescription &desc);
    D3D12_UNORDERED_ACCESS_VIEW_DESC CreateTextureUavView(const Graphics::StorageImageView &view);

    void GetShaderAccessModifiers(Graphics::StorageResourceAccess access, bool &readonly, bool &byteAddress);

    enum class RootParameterType
    {
        SamplerHeapRange,
        CBV_SRV_UAV_HeapRange,
        RootConstants,
        RootCBV,
        RootSRV,
        RootUAV
    };

    struct RootParameterBindingLocation
    {
        RootParameterType ParameterType = {};
        uint32_t RootParameterIndex = 0;
        uint32_t DescriptorOffset = 0;
    };

    struct RootSignatureBindingLocations
    {
        std::vector<RootParameterBindingLocation> HeapBindings = {};
        std::map<std::string, RootParameterBindingLocation> DynamicResources = {};
    };

    // pipeline
    void CreateRootSignature(const std::map<std::string, Graphics::ShaderResource> &reflectedResources,
                             const Graphics::ResourceSetDescription &requestedResources,
                             Microsoft::WRL::ComPtr<ID3D12Device9> device,
                             Microsoft::WRL::ComPtr<ID3DBlob> &inRootSignatureBlob,
                             Microsoft::WRL::ComPtr<ID3D12RootSignature> &inRootSignature,
                             DescriptorHandleInfo &descriptorHandleInfo,
                             RootSignatureBindingLocations &rootSignatureBindingLocation, bool requiresInputAssembly);

    std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout(const std::vector<Graphics::VertexBufferLayout> &layouts);
    D3D_PRIMITIVE_TOPOLOGY CreatePrimitiveTopology(Graphics::Topology topology);
    D3D12_RASTERIZER_DESC CreateRasterizerState(const Graphics::RasterizerStateDescription &rasterizerState);
    D3D12_STREAM_OUTPUT_DESC CreateStreamOutputDesc();
    D3D12_BLEND_DESC CreateBlendStateDesc(const std::array<Graphics::BlendStateDescription, 8> &colourBlendStates);
    D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc(const Graphics::DepthStencilDescription &depthStencilDesc);
    D3D12_DEPTH_STENCIL_DESC1 CreateDepthStencilDesc1(const Graphics::DepthStencilDescription &depthStencilDesc);

    UINT GetSyncIntervalFromPresentMode(Graphics::PresentMode presentMode);
    D3D12_RESOURCE_STATES GetTextureResourceState(Graphics::TextureLayout layout);
    D3D12_BARRIER_SYNC GetBarrierSync(Graphics::BarrierPipelineStage stage);
    D3D12_BARRIER_ACCESS GetBarrierAccess(Graphics::BarrierAccess access);
    D3D12_BARRIER_LAYOUT GetBarrierLayout(Graphics::TextureLayout layout);

    D3D12_RESOURCE_FLAGS GetResourceFlags(uint8_t usage);

    void GetD3D12AccelerationStructureInputs(const Graphics::AccelerationStructureGeometryBuildDescription &description,
                                             D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &inputs,
                                             std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> &geometry);

    void GetD3D12FeatureLevelAsMajorMinor(D3D_FEATURE_LEVEL level, uint32_t &major, uint32_t &minor);

} // namespace Nexus::D3D12

#endif