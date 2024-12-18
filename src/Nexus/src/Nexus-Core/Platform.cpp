#include "Platform.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Platform
{
	const char *GetSystemName()
	{
		return TOSTRING(NX_SYSTEM_NAME);
	}

	const char *GetBuildConfiguration()
	{
		return TOSTRING(NX_BUILD_TYPE);
	}

}	 // namespace Nexus::Platform