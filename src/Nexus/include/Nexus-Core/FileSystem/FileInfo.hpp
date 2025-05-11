#pragma once

#include "Nexus-Core/Timings/Timespan.hpp"

namespace Nexus::FileSystem
{
	enum class PathType
	{
		Invalid,
		File,
		Folder,
		Other
	};

	struct PathInfo
	{
	};
}	 // namespace Nexus::FileSystem