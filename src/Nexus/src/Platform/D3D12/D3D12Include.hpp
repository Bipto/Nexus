#if defined(NX_PLATFORM_D3D12)

	#include <Platform/Windows/WindowsInclude.hpp>

	#include <comdef.h>
	#include <d3d12.h>
	#include <d3d12sdklayers.h>
	#include <dxcapi.h>
	#include <dxgi1_6.h>
	#include <dxgidebug.h>
	#include <wrl/client.h>

	#pragma comment(lib, "user32")
	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")
	#pragma comment(lib, "dxcompiler.lib")

	#include "D3D12MemAlloc.h"

	#define BUFFER_COUNT 2

#endif