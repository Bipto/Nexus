#if !defined(__EMSCRIPTEN__)

	#include "Nexus-Core/Graphics/RenderDocAPI.hpp"

namespace Nexus::Graphics
{
	RenderDocAPI::RenderDocAPI(const std::string &libraryPath) : m_Filepath(libraryPath)
	{
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
		if (!m_Loaded)
		{
			return;
		}

		m_API->StartFrameCapture(nullptr, nullptr);
	}

	void RenderDocAPI::EndCapture()
	{
		if (!m_Loaded)
		{
			return;
		}

		m_API->EndFrameCapture(nullptr, nullptr);
	}

	void RenderDocAPI::TriggerFrameCapture()
	{
		if (!m_Loaded)
		{
			return;
		}

		m_API->TriggerCapture();
	}

	void RenderDocAPI::LaunchReplayUI()
	{
		if (!m_Loaded)
		{
			return;
		}

		m_API->LaunchReplayUI(1, nullptr);
	}

	void RenderDocAPI::SetCapturePath(const std::string &path)
	{
		m_API->SetCaptureFilePathTemplate(path.c_str());
	}
}	 // namespace Nexus::Graphics

#endif