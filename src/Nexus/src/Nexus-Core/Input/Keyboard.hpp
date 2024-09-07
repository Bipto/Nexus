#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus {
/// @brief An enum representing the keys contained on a keyboard
enum class KeyCode {
  Unknown = 0,

  Escape,

  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,

  PrintScreen,
  ScrollLock,
  PauseBreak,

  Tilde,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Zero,
  Underscore,
  Equals,
  Back,

  Tab,
  Q,
  W,
  E,
  R,
  T,
  Y,
  U,
  I,
  O,
  P,
  LeftBracket,
  RightBracket,
  Enter,

  CapsLock,
  A,
  S,
  D,
  F,
  G,
  H,
  J,
  K,
  L,
  SemiColon,
  Apostrophe,
  Hash,

  LeftShift,
  Backslash,
  Z,
  X,
  C,
  V,
  B,
  N,
  M,
  Comma,
  Period,
  Slash,
  RightShift,

  LeftControl,
  LeftWin,
  LeftAlt,
  Space,
  RightAlt,
  RightWin,
  RightControl,

  Insert,
  Home,
  PageUp,
  Delete,
  End,
  PageDown,

  KeyLeft,
  KeyUp,
  KeyDown,
  KeyRight,

  NumLock,
  KeyDivide,
  NumMultiply,
  NumSubstract,
  Num7,
  Num8,
  Num9,
  NumPlus,
  Num4,
  Num5,
  Num6,
  Num1,
  Num2,
  Num3,
  NumEnter,
  Num0,
  NumDelete,
};

/// @brief A class containing the current state of a set of keys
class Keyboard {
public:
  /// @brief A method that caches the previous frames key states
  void CacheInput();

  /// @brief A method that checks whether a key is currently held
  /// @param code The keycode to check the state of
  /// @return A boolean value representing whether a key was held
  const bool IsKeyHeld(KeyCode code) const;

  /// @brief A method that checks whether a key is currently pressed
  /// @param code The keycode to check the state of
  /// @return A boolean value representing whether a key was pressed
  const bool WasKeyPressed(KeyCode code) const;

  /// @brief A method that checks whether a key is currently released
  /// @param code The keycode to check the state of
  /// @return A boolean value representing whether a key was released
  const bool WasKeyReleased(KeyCode code) const;

  const std::map<KeyCode, bool> &GetKeys() const;

private:
  /// @brief A map containing the current state of the keyboard's keys
  mutable std::map<KeyCode, bool> m_CurrentKeys;

  /// @brief A map containing the state of the keyboard's keys in the previous
  /// frame
  mutable std::map<KeyCode, bool> m_PreviousKeys;

  /// @brief A friend class to allow a window to access the private properties
  /// of this class
  friend class Window;

  friend class Application;
};
} // namespace Nexus