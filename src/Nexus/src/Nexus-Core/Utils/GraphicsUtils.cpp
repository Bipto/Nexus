#include "Nexus-Core/Utils/GraphicsUtils.hpp"

namespace Nexus::Utils
{
	Ref<Graphics::ISurface> CreateSurfaceForWindow(Graphics::IGraphicsDevice *graphicsDevice, Nexus::IWindow *window)
	{
#if defined(WIN32)
		auto win32Info = window->GetWin32Info();
		return graphicsDevice->CreateSurfaceFromWin32(win32Info.hWND, win32Info.hDC, win32Info.hINSTANCE);
#endif

		throw std::runtime_error("Failed to create surface for window: Unsupported platform");
	}
}	 // namespace Nexus::Utils