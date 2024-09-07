#pragma once

namespace Nexus {
template <typename T> struct Buffer {
  T *Data = nullptr;
  size_t Size = 0;
};
} // namespace Nexus