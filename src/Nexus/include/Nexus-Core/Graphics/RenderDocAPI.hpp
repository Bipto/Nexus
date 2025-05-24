#pragma once

#include "Nexus-Core/Platform.hpp"
#include "Nexus-Core/nxpch.hpp"

#include <renderdoc_app.h>

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

	  private:
		std::string							  m_Filepath	  = {};
		std::unique_ptr<Utils::SharedLibrary> m_SharedLibrary = nullptr;
		RENDERDOC_API_1_6_0					 *m_API			  = nullptr;
		bool								  m_Loaded		  = false;
	};
}	 // namespace Nexus::Graphics