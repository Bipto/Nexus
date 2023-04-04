#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "SDL_syswm.h"
#include "DX11.h"

#include "BufferDX11.h"
#include "ShaderDX11.h"

namespace Nexus
{
    class GraphicsDeviceDX11 : public GraphicsDevice
    {
        public:
            GraphicsDeviceDX11(Nexus::Window* window, GraphicsAPI api) : GraphicsDevice(window, api)
            {
                #if defined(WIN32)
                SDL_SysWMinfo wmInfo;
                SDL_VERSION(&wmInfo.version);
                SDL_GetWindowWMInfo(window->GetSDLWindowHandle(), &wmInfo);
                HWND hwnd = wmInfo.info.win.window;

                DXGI_SWAP_CHAIN_DESC swap_chain_desc = {0};
                swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
                swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
                swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                swap_chain_desc.SampleDesc.Count = 1;
                swap_chain_desc.SampleDesc.Quality = 0;
                swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swap_chain_desc.BufferCount = 1;
                swap_chain_desc.OutputWindow = hwnd;
                swap_chain_desc.Windowed = true;

                D3D_FEATURE_LEVEL feature_level;
                UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
                #if defined(DEBUG) | defined(_DEBUG)
                flags |= D3D11_CREATE_DEVICE_DEBUG;
                #endif

                HRESULT hr = D3D11CreateDeviceAndSwapChain(
                    NULL,
                    D3D_DRIVER_TYPE_HARDWARE,
                    NULL,
                    flags,
                    NULL,
                    0,
                    D3D11_SDK_VERSION,
                    &swap_chain_desc,
                    &m_SwapChainPtr,
                    &m_DevicePtr,
                    &feature_level,
                    &m_DeviceContextPtr
                );

                ID3D11Texture2D* framebuffer;
                hr = m_SwapChainPtr->GetBuffer(
                    0,
                    _uuidof(ID3D11Texture2D),
                    (void**)&framebuffer 
                );

                hr = m_DevicePtr->CreateRenderTargetView(
                    framebuffer, 0, &m_RenderTargetViewPtr
                );

                framebuffer->Release();

                #endif
            }

            virtual void SetContext() override
            {

            }

            virtual void Clear(float red, float green, float blue, float alpha)
            {
                #if defined(WIN32)
                float backgroundColor[4] = {red, green, blue, alpha};
                m_DeviceContextPtr->ClearRenderTargetView(
                    m_RenderTargetViewPtr, backgroundColor  
                );
                #endif
            }

            virtual void DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader)
            {
                shader->Bind();

                Ref<VertexBufferDX11> vb = std::dynamic_pointer_cast<VertexBufferDX11>(vertexBuffer);

                auto layout = shader->GetLayout();
                ID3D11Buffer* buffer = vb->GetNativeHandle();
                uint32_t stride = sizeof(float) * 3;
                uint32_t offset = 0;

                m_DeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                m_DeviceContextPtr->IASetVertexBuffers(
                    0,
                    1,
                    &buffer,
                    &stride,
                    &offset
                );
                m_DeviceContextPtr->Draw(3, 0);
            }

            virtual void DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer)
            {

            }

            virtual const char* GetAPIName()
            {
                return "DirectX11";
            }

            virtual const char* GetDeviceName()
            {
                return "Test";
            }

            virtual void* GetContext()
            {
                return m_DeviceContextPtr;
            }

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
            {
                return {};
            }
            
            virtual Ref<Shader> CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout)
            {
                return CreateRef<ShaderDX11>(m_DevicePtr, m_DeviceContextPtr, filepath, layout); 
            }
            
            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices)
            {
                return CreateRef<VertexBufferDX11>(m_DevicePtr, vertices);
            }

            virtual Ref<IndexBuffer> CreateIndexBuffer(unsigned int indices[], unsigned int indexCount)
            {
                return {};
            }

            virtual Ref<Texture> CreateTexture(const char* filepath)
            {
                return {};
            }
            
            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec)
            {
                return {};
            }

            virtual void Resize(Point size)
            {
                #if defined(WIN32)

                m_DeviceContextPtr->OMSetRenderTargets(0, 0, 0);
                m_RenderTargetViewPtr->Release();

                HRESULT hr;
                hr = m_SwapChainPtr->ResizeBuffers(0, size.Width, size.Height, DXGI_FORMAT_UNKNOWN, 0);

                ID3D11Texture2D* pBuffer;
                hr = m_SwapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
                hr = m_DevicePtr->CreateRenderTargetView(pBuffer, NULL, &m_RenderTargetViewPtr);
                pBuffer->Release();

                m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, NULL);

                D3D11_VIEWPORT vp;
                vp.Width = (float)size.Width;
                vp.Height = (float)size.Height;
                vp.MinDepth = 0.0f;
                vp.MaxDepth = 1.0f;
                vp.TopLeftX = 0;
                vp.TopLeftY = 0;
                m_DeviceContextPtr->RSSetViewports(1, &vp);

                #endif
            }

            virtual void SwapBuffers()
            {
                #if defined(WIN32)
                m_SwapChainPtr->Present(m_VsyncValue, 0);
                #endif
            }

            virtual void SetVSyncState(VSyncState vSyncState)
            {
                #if defined(WIN32)
                switch (vSyncState)
                {
                    case VSyncState::Enabled:
                        m_VsyncValue = 1;
                        break;
                    default:
                        m_VsyncValue = 0;
                        break;
                }
                #endif
            }

        #if defined(WIN32)
        private:
            ID3D11Device* m_DevicePtr                       = NULL;
            ID3D11DeviceContext* m_DeviceContextPtr         = NULL;
            IDXGISwapChain* m_SwapChainPtr                  = NULL;
            ID3D11RenderTargetView* m_RenderTargetViewPtr   = NULL;
            unsigned int m_VsyncValue                       = 1;
        #endif
    };
}