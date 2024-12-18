#pragma once

namespace Nexus
{
	template<typename T>
	struct Buffer
	{
		T	  *Data = nullptr;
		size_t Count = 0;

		size_t GetSizeInBytes() const
		{
			return sizeof(T) * Count;
		}
	};
}	 // namespace Nexus