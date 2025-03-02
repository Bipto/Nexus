#include "Nexus-Core/Graphics/Multisample.hpp"

#include "Nexus-Core/nxpch.hpp"

uint32_t Nexus::Graphics::GetSampleCount(SampleCount samples)
{
	switch (samples)
	{
		case SampleCount::SampleCount1: return 1;
		case SampleCount::SampleCount2: return 2;
		case SampleCount::SampleCount4: return 4;
		case SampleCount::SampleCount8: return 8;
		default: throw std::runtime_error("Invalid sample count entered");
	}
}