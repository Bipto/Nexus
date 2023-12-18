#pragma once

#include "Nexus/Graphics/Pipeline.hpp"

#include "D3D11Include.hpp"

#if defined(NX_PLATFORM_D3D11)

namespace Nexus::Graphics
{

    class PipelineD3D11 : public Pipeline
    {
    public:
        PipelineD3D11(ID3D11Device *device, ID3D11DeviceContext *context, const PipelineDescription &description);
        virtual ~PipelineD3D11();
        virtual const PipelineDescription &GetPipelineDescription() const override;

    public:
        ID3D11DepthStencilState *GetDepthStencilState() { return m_DepthStencilState; }
        ID3D11RasterizerState *GetRasterizerState() { return m_RasterizerState; }
        ID3D11BlendState *GetBlendState() { return m_BlendState; }
        D3D11_PRIMITIVE_TOPOLOGY GetTopology();

    private:
        void SetupDepthStencilState();
        void SetupRasterizerState();
        void SetupBlendState();

    private:
        ID3D11Device *m_Device;
        ID3D11DeviceContext *m_Context;
        ID3D11DepthStencilState *m_DepthStencilState = NULL;
        ID3D11RasterizerState *m_RasterizerState = NULL;
        ID3D11BlendState *m_BlendState = NULL;
    };

}
#endif