#if defined(NX_PLATFORM_WGL)

	#include "Nexus-Core/nxpch.hpp"

	#include "Platform/OpenGL/Context/WGL/PhysicalDeviceWGL.hpp"
	#include "Platform/OpenGL/GL.hpp"

namespace Nexus::GL
{
	void LoadGLFunctionsIfNeeded(HDC hdc)
	{
		if (!gladLoadWGL(hdc))
		{
			MessageBox(NULL, "Failed to load WGL", "Error", MB_OK);
		}

		if (!gladLoadGL())
		{
			MessageBox(NULL, "Failed to load OpenGL function pointers", "Error", MB_OK);
		}
	}

	std::tuple<HWND, HGLRC, HDC> CreateTemporaryWindow()
	{
		const char className[] = "Temporary OpenGL Window";

		WNDCLASS wc		 = {};
		wc.lpfnWndProc	 = DefWindowProc;
		wc.hInstance	 = NULL;
		wc.lpszClassName = className;
		wc.style		 = CS_OWNDC;
		RegisterClass(&wc);

		HWND hwnd = CreateWindowEx(0,
								   className,
								   "TempWindow",
								   WS_OVERLAPPEDWINDOW,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   NULL,
								   NULL,
								   NULL,
								   NULL);

		if (!IsWindow(hwnd))
		{
			MessageBox(NULL, "Could not create window", "Error", MB_OK);
		}

		ShowWindow(hwnd, SW_HIDE);

		HDC hdc = GetDC(hwnd);

		PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
									 1,
									 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
									 PFD_TYPE_RGBA,
									 32,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 24,
									 8,
									 0,
									 PFD_MAIN_PLANE,
									 0,
									 0,
									 0,
									 0};

		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		HGLRC hglrc = wglCreateContext(hdc);

		NX_ASSERT(hglrc, "Failed to create HGLRC");

		return {hwnd, hglrc, hdc};
	}

	bool LoadOpenGL()
	{
		auto [tempWindow, tempHglrc, tempHdc] = CreateTemporaryWindow();
		wglMakeCurrent(tempHdc, tempHglrc);
		LoadGLFunctionsIfNeeded(tempHdc);

		DeleteDC(tempHdc);

		return true;
	}

	std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices()
	{
		std::map<std::string, std::vector<std::string>> displays;

		DISPLAY_DEVICE displayDevice;
		displayDevice.cb  = sizeof(DISPLAY_DEVICE);
		DWORD deviceIndex = 0;

		while (EnumDisplayDevices(nullptr, deviceIndex, &displayDevice, 0))
		{
			std::string deviceString = displayDevice.DeviceString;
			std::string deviceName	 = displayDevice.DeviceName;
			displays[deviceString].push_back(deviceName);

			deviceIndex++;
		}

		std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> physicalDevices;
		for (const auto &[name, attachedDisplays] : displays)
		{
			std::shared_ptr<Graphics::IPhysicalDevice> physicalDevice = std::make_shared<Graphics::PhysicalDeviceWGL>(name, attachedDisplays);
			physicalDevices.push_back(physicalDevice);
		}

		return physicalDevices;
	}
}	 // namespace Nexus::GL

#endif