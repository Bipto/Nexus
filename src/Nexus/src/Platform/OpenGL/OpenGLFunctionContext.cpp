#include "OpenGLFunctionContext.hpp"

namespace Nexus::GL
{
	OpenGLFunctionContext::OpenGLFunctionContext()
	{
	}

	OpenGLFunctionContext::~OpenGLFunctionContext()
	{
		gladLoaderUnloadGLContext(&m_Context);
	}

	bool OpenGLFunctionContext::Load()
	{
		int result = gladLoaderLoadGLContext(&m_Context);
		return result;
	}

	void OpenGLFunctionContext::ExecuteCommands(std::function<void(const GladGLContext &context)> function)
	{
		function(m_Context);
	}

}	 // namespace Nexus::GL
