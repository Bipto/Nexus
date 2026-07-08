#include "Nexus-Core/Graphics/RenderDocAPI.hpp"

#include "Platform/Platform.hpp"

namespace Nexus::Graphics
{
    RenderDocAPI::RenderDocAPI(const std::string &libraryPath) : m_Filepath(libraryPath)
    {
#if defined(NX_USE_RENDERDOC)
        m_SharedLibrary = std::unique_ptr<Utils::SharedLibrary>(Platform::LoadSharedLibrary(m_Filepath));
        if (m_SharedLibrary)
        {
            auto RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)(m_SharedLibrary->LoadSymbol("RENDERDOC_GetAPI"));
            if (RENDERDOC_GetAPI)
            {
                int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void **)&m_API);
                if (ret == 1 && m_API)
                {
                    m_Loaded = true;
                }
            }
        }
#endif
    }

    RenderDocAPI::~RenderDocAPI()
    {
    }

    bool RenderDocAPI::IsLoaded() const
    {
        return m_Loaded;
    }

    void RenderDocAPI::StartCapture()
    {
#if defined(NX_USE_RENDERDOC)
        if (!m_Loaded)
        {
            return;
        }

        m_API->StartFrameCapture(nullptr, nullptr);
#endif
    }

    void RenderDocAPI::EndCapture()
    {
#if defined(NX_USE_RENDERDOC)
        if (!m_Loaded)
        {
            return;
        }

        m_API->EndFrameCapture(nullptr, nullptr);
#endif
    }

    void RenderDocAPI::TriggerFrameCapture()
    {
#if defined(NX_USE_RENDERDOC)
        if (!m_Loaded)
        {
            return;
        }

        m_API->TriggerCapture();
#endif
    }

    void RenderDocAPI::LaunchReplayUI()
    {
#if defined(NX_USE_RENDERDOC)
        if (!m_Loaded)
        {
            return;
        }

        m_API->LaunchReplayUI(1, nullptr);
#endif
    }

    void RenderDocAPI::SetCapturePath(const std::string &path)
    {
#if defined(NX_USE_RENDERDOC)
        m_API->SetCaptureFilePathTemplate(path.c_str());
#endif
    }

    bool RenderDocAPI::IsSupported() const
    {
#if defined(NX_USE_RENDERDOC)
        return true;
#else
        return false;
#endif
    }
} // namespace Nexus::Graphics