#pragma once

#include <memory>

#if defined(NX_USE_RENDERDOC)
#include <renderdoc_app.h>
#endif

#include "Nexus-Core/nxpch.hpp"
#include "Platform/Utils/SharedLibrary.hpp"

namespace Nexus::Graphics
{
    class NX_API RenderDocAPI
    {
      public:
        RenderDocAPI(const std::string &libraryPath);
        virtual ~RenderDocAPI();
        bool IsLoaded() const;
        void StartCapture();
        void EndCapture();
        void TriggerFrameCapture();
        void LaunchReplayUI();
        void SetCapturePath(const std::string &path);
        bool IsSupported() const;

      private:
        std::string m_Filepath = {};
        bool m_Loaded = false;
#if defined(NX_USE_RENDERDOC)
        std::unique_ptr<Utils::SharedLibrary> m_SharedLibrary = nullptr;
        RENDERDOC_API_1_6_0 *m_API = nullptr;
#endif
    };
} // namespace Nexus::Graphics