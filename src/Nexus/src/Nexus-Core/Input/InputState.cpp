#include "InputState.hpp"

namespace Nexus {
void InputState::CacheInput() {
  m_Keyboard.CacheInput();
  m_Mouse.CacheInput();
}
} // namespace Nexus