#include "Input.hpp"

#include "Nexus-Core/Runtime.hpp"

namespace Nexus {
// instatiate static vector
std::vector<Gamepad *> Input::s_Gamepads;

const InputState *Input::s_InputContext = nullptr;

bool Input::IsLeftMousePressed() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.WasLeftMouseClicked();
}

bool Input::IsLeftMouseReleased() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.WasLeftMouseReleased();
}

bool Input::IsLeftMouseHeld() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.IsLeftMouseHeld();
}

bool Input::IsRightMousePressed() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.WasRightMouseClicked();
}

bool Input::IsRightMouseReleased() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.WasRightMouseReleased();
}

bool Input::IsRightMouseHeld() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.IsRightMouseHeld();
}

bool Input::IsMiddleMousePressed() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.IsMiddleMouseHeld();
}

bool Input::IsMiddleMouseReleased() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.WasMiddleMouseReleased();
}

bool Input::IsMiddleMouseHeld() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.IsMiddleMouseHeld();
}

Point2D<int> Input::GetMousePosition() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.GetMousePosition();
}

Point2D<int> Input::GetMouseMovement() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.GetMouseMovement();
}

float Input::GetMouseScrollX() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.GetScroll().X;
}

float Input::GetMouseScrollMovementX() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.GetScrollMovement().X;
}

float Input::GetMouseScrollY() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.GetScroll().Y;
}

float Input::GetMouseScrollMovementY() {
  auto mouse = s_InputContext->GetMouse();
  return mouse.GetScrollMovement().Y;
}

bool Input::IsGamepadConnected() { return s_Gamepads.size() > 0; }

int Input::GetGamepadCount() { return s_Gamepads.size(); }

bool Input::IsGamepadKeyHeld(uint32_t index, GamepadButton button) {
  if (GetGamepadCount() == 0)
    return false;
  return s_Gamepads[index]->IsButtonHeld(button);
}

bool Input::WasGamepadKeyPressed(uint32_t index, GamepadButton button) {
  if (GetGamepadCount() == 0)
    return false;
  return s_Gamepads[index]->WasButtonPressed(button);
}

bool Input::WasGamepadKeyReleased(uint32_t index, GamepadButton button) {
  if (GetGamepadCount() == 0)
    return false;
  return s_Gamepads[index]->WasButtonReleased(button);
}

Point2D<float> Input::GetGamepadAxisLeft(uint32_t index) {
  if (GetGamepadCount() == 0)
    return {0, 0};

  return s_Gamepads[index]->GetLeftStick();
}

Point2D<float> Input::GetGamepadAxisRight(uint32_t index) {
  if (GetGamepadCount() == 0)
    return {0, 0};

  return s_Gamepads[index]->GetRightStick();
}

float Input::GetGamepadLeftTrigger(uint32_t index) {
  if (GetGamepadCount() == 0)
    return 0;

  return s_Gamepads[index]->GetLeftTrigger();
}

float Input::GetGamepadRightTrigger(uint32_t index) {
  if (GetGamepadCount() == 0)
    return 0;

  return s_Gamepads[index]->GetRightTrigger();
}

void Input::GamepadRumble(uint32_t index, uint16_t lowFrequency,
                          uint16_t highFrequency, uint32_t milliseconds) {
  if (index > GetGamepadCount()) {
    s_Gamepads[index]->Rumble(lowFrequency, highFrequency, milliseconds);
  }
}

void Input::GamepadRumbleTriggers(uint32_t index, uint16_t left, uint16_t right,
                                  uint32_t milliseconds) {
  if (GetGamepadCount() == 0)
    return;

  return s_Gamepads[index]->RumbleTriggers(left, right, milliseconds);
}

void Input::GamepadSetLED(uint32_t index, uint8_t red, uint8_t green,
                          uint8_t blue) {
  if (GetGamepadCount() == 0)
    return;

  return s_Gamepads[index]->SetLED(red, green, blue);
}

void Input::SetInputContext(const InputState *input) { s_InputContext = input; }

const InputState *Input::GetCurrentInputContext() { return s_InputContext; }

Gamepad *Input::GetGamepadByIndex(uint32_t index) {
  for (int i = 0; i < s_Gamepads.size(); i++) {
    auto gamepad = s_Gamepads[i];
    if (gamepad->GetControllerIndex() == index) {
      return gamepad;
    }
  }

  return nullptr;
}

void Input::AddController(uint32_t index) {
  Gamepad *gamepad = new Gamepad(index);
  s_Gamepads.push_back(gamepad);
}

void Input::RemoveController(uint32_t index) {
  for (int i = 0; i < s_Gamepads.size(); i++) {
    auto gamepad = s_Gamepads[i];
    if (gamepad->GetControllerIndex() == index) {
      delete gamepad;
      s_Gamepads.erase(s_Gamepads.begin() + i);
    }
  }
}

void Input::CacheInput() {
  for (int i = 0; i < s_Gamepads.size(); i++) {
    s_Gamepads[i]->Update();
  }
}

bool Input::IsKeyPressed(KeyCode code) {
  auto keyboard = s_InputContext->GetKeyboard();
  return keyboard.WasKeyPressed(code);
}

bool Input::IsKeyReleased(KeyCode code) {
  auto keyboard = s_InputContext->GetKeyboard();
  return keyboard.WasKeyReleased(code);
}

bool Input::IsKeyHeld(KeyCode code) {
  auto keyboard = s_InputContext->GetKeyboard();
  return keyboard.IsKeyHeld(code);
}
} // namespace Nexus