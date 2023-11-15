#include "ImGuiRenderer.hpp"

#include "Nexus/Application.hpp"
#include "Platform/D3D11/GraphicsDeviceD3D11.hpp"
#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"
#include "Platform/Vulkan/GraphicsDeviceVk.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_sdl2.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_dx11.h>

#if defined(NX_PLATFORM_VULKAN)
#include <backends/imgui_impl_vulkan.h>
#include "Platform/Vulkan/TextureVk.hpp"
#include "Platform/Vulkan/FramebufferVk.hpp"
#endif

#include <backends/imgui_impl_dx12.h>
#include "Platform/D3D12/TextureD3D12.hpp"
#include "Platform/D3D12/FramebufferD3D12.hpp"

namespace Nexus::Graphics
{
    ImGuiRenderer::ImGuiRenderer(Application *app)
    {
        m_Application = app;
    }

    void ImGuiRenderer::Initialise()
    {
        auto graphicsDevice = m_Application->GetGraphicsDevice();
        switch (graphicsDevice->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            InitialiseOpenGL();
            break;
        case GraphicsAPI::D3D11:
            InitialiseD3D11();
            break;
        case GraphicsAPI::D3D12:
            InitialiseD3D12();
            break;
        case GraphicsAPI::Vulkan:
            InitialiseVulkan();
            break;
        }
    }

    void ImGuiRenderer::Shutdown()
    {
        auto graphicsDevice = m_Application->GetGraphicsDevice();
        switch (graphicsDevice->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_Shutdown();
            break;
        case GraphicsAPI::D3D11:
#if defined(NX_PLATFORM_D3D11)
            ImGui_ImplDX11_Shutdown();
            break;
#endif
        case GraphicsAPI::D3D12:
            ImGui_ImplDX12_Shutdown();
            break;
        case GraphicsAPI::Vulkan:
#if defined(NX_PLATFORM_VULKAN)
            ImGui_ImplVulkan_Shutdown();
#endif
            break;
        }
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplSDL2_NewFrame();
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        case GraphicsAPI::D3D11:
#if defined(NX_PLATFORM_D3D11)
            ImGui_ImplDX11_NewFrame();
#endif
            break;
        case GraphicsAPI::D3D12:
            ImGui_ImplDX12_NewFrame();
            break;
        case GraphicsAPI::Vulkan:
#if defined(NX_PLATFORM_VULKAN)
            ImGui_ImplVulkan_NewFrame();
#endif
            break;
        }
        ImGui::NewFrame();
    }

    void ImGuiRenderer::EndFrame()
    {
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
        case GraphicsAPI::OpenGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        case GraphicsAPI::D3D11:
#if defined(NX_PLATFORM_D3D11)
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
            break;
        case GraphicsAPI::D3D12:
            EndFrameImplD3D12();
            break;
        case GraphicsAPI::Vulkan:
#if defined(NX_PLATFORM_VULKAN)
            EndFrameImplVulkan();
#endif
            break;
        }
    }

    void ImGuiRenderer::UpdatePlatformWindows()
    {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            m_Application->GetGraphicsDevice()->SetContext();
        }
    }

    void ImGuiRenderer::RebuildFontAtlas()
    {
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
#if defined(NX_PLATFORM_VULKAN)
        case GraphicsAPI::Vulkan:
        {
            auto vulkanGraphicsDevice = (GraphicsDeviceVk *)m_Application->GetGraphicsDevice();
            vulkanGraphicsDevice->ImmediateSubmit([&](VkCommandBuffer cmd)
                                                  { ImGui_ImplVulkan_CreateFontsTexture(cmd); });
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }

#endif
        }
    }

    ImTextureID ImGuiRenderer::BindTexture(Texture *texture)
    {
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
#if defined(NX_PLATFORM_VULKAN)
        case GraphicsAPI::Vulkan:
        {
            auto textureVk = (TextureVk *)texture;
            return ImGui_ImplVulkan_AddTexture(textureVk->GetSampler(), textureVk->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
#endif

        case GraphicsAPI::D3D12:
        {
            GraphicsDeviceD3D12 *graphicsDevice = (GraphicsDeviceD3D12 *)m_Application->GetGraphicsDevice();
            auto d3d12Device = graphicsDevice->GetDevice();

            auto d3d12Texture = (TextureD3D12 *)texture;

            auto descriptorHeap = graphicsDevice->GetImGuiDescriptorHeap();
            auto descriptorIndex = graphicsDevice->GetNextTextureOffset();

            D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
            D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
            srv_cpu_handle.ptr += (descriptorIndex * d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            srv_gpu_handle.ptr += (descriptorIndex * d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Format = d3d12Texture->GetFormat();
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.PlaneSlice = 0;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            d3d12Device->CreateShaderResourceView(d3d12Texture->GetD3D12ResourceHandle(), &srvDesc, srv_cpu_handle);

            return (ImTextureID)srv_gpu_handle.ptr;
        }

        default:
            return (ImTextureID)texture->GetHandle();
        }
    }

    ImTextureID ImGuiRenderer::BindFramebufferTexture(Framebuffer *framebuffer, uint32_t textureIndex)
    {
        switch (m_Application->GetGraphicsDevice()->GetGraphicsAPI())
        {
#if defined(NX_PLATFORM_VULKAN)
        case GraphicsAPI::Vulkan:
        {
            auto framebufferVk = (FramebufferVk *)framebuffer;
            auto sampler = framebufferVk->GetColorTextureSampler(textureIndex);
            auto imageView = framebufferVk->GetColorTextureImageView(textureIndex);
            return ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_GENERAL);
        }
#endif
        case GraphicsAPI::D3D12:
        {
            GraphicsDeviceD3D12 *graphicsDevice = (GraphicsDeviceD3D12 *)m_Application->GetGraphicsDevice();
            auto d3d12Device = graphicsDevice->GetDevice();

            auto d3d12Framebuffer = (FramebufferD3D12 *)framebuffer;

            auto descriptorHeap = graphicsDevice->GetImGuiDescriptorHeap();
            auto descriptorIndex = graphicsDevice->GetNextTextureOffset();

            D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
            D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
            srv_cpu_handle.ptr += (descriptorIndex * d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            srv_gpu_handle.ptr += (descriptorIndex * d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
            srvDesc.Format = d3d12Framebuffer->GetColorAttachmentFormat(textureIndex);
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.PlaneSlice = 0;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

            ID3D12Resource2 *resource = (ID3D12Resource2 *)d3d12Framebuffer->GetColorAttachment(textureIndex);
            d3d12Device->CreateShaderResourceView(resource, &srvDesc, srv_cpu_handle);

            return (ImTextureID)srv_gpu_handle.ptr;
        }
        default:
            return framebuffer->GetColorAttachment(textureIndex);
        }
    }

    void ImGuiRenderer::InitialiseOpenGL()
    {
        const char *glslVersion;
        // Decide GL+GLSL versions
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
        // GL ES 2.0 + GLSL 100
        glslVersion = "#version 100";
#elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        glslVersion = "#version 150";
#else
        // GL 3.0 + GLSL 130
        glslVersion = "#version 130";
#endif

        SDL_Window *window = m_Application->GetPrimaryWindow()->GetSDLWindowHandle();
        SDL_GLContext context = (SDL_GLContext)m_Application->GetGraphicsDevice()->GetContext();
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init(glslVersion);
    }

    void ImGuiRenderer::InitialiseD3D11()
    {
#if defined(NX_PLATFORM_D3D11)
        ImGui_ImplSDL2_InitForD3D(m_Application->GetPrimaryWindow()->GetSDLWindowHandle());
        auto device = (GraphicsDeviceD3D11 *)(m_Application->GetGraphicsDevice());
        ID3D11Device *id3d11Device = (ID3D11Device *)device->GetDevice();
        ID3D11DeviceContext *id3d11DeviceContext = (ID3D11DeviceContext *)device->GetDeviceContext();
        ImGui_ImplDX11_Init(id3d11Device, id3d11DeviceContext);
#endif
    }

    void ImGuiRenderer::InitialiseD3D12()
    {
        ImGui_ImplSDL2_InitForD3D(m_Application->GetPrimaryWindow()->GetSDLWindowHandle());
        auto device = (GraphicsDeviceD3D12 *)(m_Application->GetGraphicsDevice());
        auto descriptorHeap = device->GetImGuiDescriptorHeap();
        ImGui_ImplDX12_Init(device->GetDevice(), 1, DXGI_FORMAT_R8G8B8A8_UNORM, descriptorHeap, descriptorHeap->GetCPUDescriptorHandleForHeapStart(), descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    void ImGuiRenderer::InitialiseVulkan()
    {
#if defined(NX_PLATFORM_VULKAN)
        auto graphicsDevice = m_Application->GetGraphicsDevice();
        auto vulkanGraphicsDevice = (GraphicsDeviceVk *)(graphicsDevice);

        VkDescriptorPoolSize poolSizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = std::size(poolSizes);
        poolInfo.pPoolSizes = poolSizes;

        VkDescriptorPool imguiPool;
        if (vkCreateDescriptorPool(vulkanGraphicsDevice->m_Device, &poolInfo, nullptr, &imguiPool))
        {
            throw std::runtime_error("Failed to create ImGui descriptor pool");
        }

        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForVulkan(m_Application->GetPrimaryWindow()->GetSDLWindowHandle());

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = vulkanGraphicsDevice->m_Instance;
        initInfo.PhysicalDevice = vulkanGraphicsDevice->m_PhysicalDevice;
        initInfo.Device = vulkanGraphicsDevice->m_Device;
        initInfo.Queue = vulkanGraphicsDevice->m_GraphicsQueue;
        initInfo.DescriptorPool = imguiPool;
        initInfo.MinImageCount = vulkanGraphicsDevice->GetSwapchainImageCount();
        initInfo.ImageCount = vulkanGraphicsDevice->GetSwapchainImageCount();
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        SwapchainVk *swapchain = (SwapchainVk *)vulkanGraphicsDevice->GetPrimaryWindow()->GetSwapchain();

        ImGui_ImplVulkan_Init(&initInfo, swapchain->GetRenderPass());
        vulkanGraphicsDevice->ImmediateSubmit([&](VkCommandBuffer cmd)
                                              { ImGui_ImplVulkan_CreateFontsTexture(cmd); });
        ImGui_ImplVulkan_DestroyFontUploadObjects();
#endif
    }

    void ImGuiRenderer::EndFrameImplVulkan()
    {
#if defined(NX_PLATFORM_VULKAN)
        auto graphicsDevice = m_Application->GetGraphicsDevice();
        auto vulkanGraphicsDevice = (GraphicsDeviceVk *)(graphicsDevice);

        vulkanGraphicsDevice->BeginImGuiRenderPass();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vulkanGraphicsDevice->m_ImGuiCommandBuffer);
        vulkanGraphicsDevice->EndImGuiRenderPass();
#endif
    }

    void ImGuiRenderer::EndFrameImplD3D12()
    {
        auto graphicsDevice = (GraphicsDeviceD3D12 *)m_Application->GetGraphicsDevice();
        auto d3d12Device = graphicsDevice->GetDevice();

        graphicsDevice->BeginImGuiFrame();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), graphicsDevice->GetImGuiCommandList());
        graphicsDevice->EndImGuiFrame();
    }
}