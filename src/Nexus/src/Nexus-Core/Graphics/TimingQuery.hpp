#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics {
class TimingQuery {
public:
  virtual ~TimingQuery() {}
  virtual void Resolve() = 0;
  virtual float GetElapsedMilliseconds() = 0;
};
} // namespace Nexus::Graphics