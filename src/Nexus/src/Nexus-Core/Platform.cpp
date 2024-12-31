#include "Platform.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Platform
{
	const char *GetSystemName()
	{
		return TOSTRING(NX_SYSTEM_NAME);
	}

	const char *GetProcessorType()
	{
		return TOSTRING(NX_SYSTEM_PROCESSOR);
	}

}	 // namespace Nexus::Platform