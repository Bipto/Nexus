#include "Platform/OpenGL/Context/Glad2/OffscreenContextGlad2.hpp"

#include "Platform/OpenGL/GL.hpp"

#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::GL
{
    OffscreenContextGlad2::OffscreenContextGlad2()
    {
    }

    OffscreenContextGlad2::~OffscreenContextGlad2()
    {
        gladLoaderUnloadGLContext(&m_Context);
    }

    bool OffscreenContextGlad2::Load()
    {
        int result = gladLoaderLoadGLContext(&m_Context);
        return result;
    }

    void OffscreenContextGlad2::ExecuteCommands(std::function<void(const GladGLContext &context)> function)
    {
        function(m_Context);
    }

    const GladGLContext &OffscreenContextGlad2::GetContext() const
    {
        return m_Context;
    }
} // namespace Nexus::GL
