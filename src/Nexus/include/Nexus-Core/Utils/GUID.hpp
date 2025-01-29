#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	struct GUID
	{
		uint64_t Value = 0;

		GUID()
		{
			static std::random_device					   rd;
			static std::mt19937_64						   e2(rd());
			static std::uniform_int_distribution<uint64_t> dist(std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));

			Value = dist(e2);
		}

		explicit GUID(uint64_t v)
		{
			Value = v;
		}

		operator uint64_t() const
		{
			return Value;
		}

		std::pair<uint32_t, uint32_t> Split()
		{
			uint32_t lowerValue = static_cast<uint32_t>(Value & 0xFFFFFFFF);
			uint32_t upperValue = static_cast<uint32_t>((Value >> 32) & 0xFFFFFFFF);
			return {lowerValue, upperValue};
		}

		static GUID FromSplit(uint32_t lowerValue, uint32_t upperValue)
		{
			uint64_t value = ((uint64_t)upperValue << 32) | lowerValue;
			return GUID(value);
		}
	};
}	 // namespace Nexus