#pragma once

#include "Core/Graphics/Pipeline.h"

#include "DX11.h"

#if defined(NX_PLATFORM_DX11)

namespace Nexus
{

    class PipelineDX11 : public Pipeline
    {
    public:
        PipelineDX11(ID3D11Device *device, const PipelineDescription &description);
        virtual ~PipelineDX11() {}
        virtual const PipelineDescription &GetPipelineDescription() const override;

    public:
        ID3D11DepthStencilState *GetDepthStencilState() { return m_DepthStencilState; }
        ID3D11RasterizerState *GetRasterizerState() { return m_RasterizerState; }
        const D3D11_RECT &GetScissorRectangle() { return m_ScissorRectangle; }
        D3D11_PRIMITIVE_TOPOLOGY GetTopology();

    private:
        void SetupDepthStencilState();
        void SetupRasterizerState();

    private:
        ID3D11Device *m_Device;
        ID3D11DepthStencilState *m_DepthStencilState = NULL;
        ID3D11RasterizerState *m_RasterizerState = NULL;
        D3D11_RECT m_ScissorRectangle;
    };

}
#endif