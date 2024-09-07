#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics {
enum class SampleCount {
  SampleCount1,
  SampleCount2,
  SampleCount4,
  SampleCount8
};

uint32_t GetSampleCount(SampleCount samples);
} // namespace Nexus::Graphics