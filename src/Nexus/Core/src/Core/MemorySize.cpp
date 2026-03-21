#include "Core/MemorySize.hpp"

#include <cmath>

namespace
{
	// The size of a SI kilobyte as bytes
	constexpr double SI_KilobytesBase = 1000.0;

	// The size of a IEC kibibytes as bytes
	constexpr double IEC_KibibytesBase = 1024.0;

	double ConvertUnitToBytes(Nexus::MemoryUnit unit)
	{
		switch (unit)
		{
			case Nexus::MemoryUnit::Bytes: return 1.0;
			case Nexus::MemoryUnit::Kilobytes: return SI_KilobytesBase;
			case Nexus::MemoryUnit::Kibibytes: return IEC_KibibytesBase;
			case Nexus::MemoryUnit::Megabytes: return std::pow(SI_KilobytesBase, 2);
			case Nexus::MemoryUnit::Mibibytes: return std::pow(IEC_KibibytesBase, 2);
			case Nexus::MemoryUnit::Gigabytes: return std::pow(SI_KilobytesBase, 3);
			case Nexus::MemoryUnit::Gibibytes: return std::pow(IEC_KibibytesBase, 3);
			case Nexus::MemoryUnit::Terabytes: return std::pow(SI_KilobytesBase, 4);
			case Nexus::MemoryUnit::Tibibytes: return std::pow(IEC_KibibytesBase, 4);
			case Nexus::MemoryUnit::Petabytes: return std ::pow(SI_KilobytesBase, 5);
			case Nexus::MemoryUnit::Pebibytes: return std ::pow(IEC_KibibytesBase, 5);
			default: return 1.0;
		}
	}
}	 // namespace

namespace Nexus
{
	double MemorySizeConverter::Convert(double value, MemoryUnit from, MemoryUnit to)
	{
		double bytes = value * ConvertUnitToBytes(from);
		return bytes / ConvertUnitToBytes(to);
	}

	MemorySize::MemorySize(double value, MemoryUnit unit) : m_Bytes(value * ConvertUnitToBytes(unit))
	{
	}

	double MemorySize::To(MemoryUnit unit) const
	{
		return m_Bytes / ConvertUnitToBytes(unit);
	}

	MemorySize operator+(const MemorySize &a, const MemorySize &b)
	{
		return MemorySize(a.To(MemoryUnit::Bytes) + b.To(MemoryUnit::Bytes), MemoryUnit::Bytes);
	}

	MemorySize operator-(const MemorySize &a, const MemorySize &b)
	{
		return MemorySize(a.To(MemoryUnit::Bytes) - b.To(MemoryUnit::Bytes), MemoryUnit::Bytes);
	}
}	 // namespace Nexus