#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "OffscreenContextWGL.hpp"
#include "Platform/OpenGL/PhysicalDeviceOpenGL.hpp"
#include "Platform/Windows/WindowsInclude.hpp"
#include "glad/wgl.h"

namespace Nexus::Graphics
{
    class PhysicalDeviceWGL final : public PhysicalDeviceOpenGL
    {
      public:
        PhysicalDeviceWGL(const std::string &deviceName, const std::vector<std::string> &displayNames, bool debug);
        virtual ~PhysicalDeviceWGL();
        const std::string &GetDeviceName() const final;
        virtual GL::IOffscreenContext *GetOffscreenContext() final;

        HDC GetHDC() const;

      private:
        std::string m_DeviceName = {};
        std::vector<std::string> m_DisplayNames = {};

        HDC m_HDC = {};
        std::unique_ptr<GL::OffscreenContextWGL> m_OffscreenContext = nullptr;
    };
} // namespace Nexus::Graphics