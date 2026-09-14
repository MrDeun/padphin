#include "KeyboardControl.hpp"
#include "imgui.h"

KeyboardControl::KeyboardControl(SDL_Window *window) : window_(window) {}

Input KeyboardControl::poll() {
  Input in;

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard) {
    up_.reset();
    down_.reset();
    left_.reset();
    right_.reset();
    return in;
  }

  int num_keys = 0;
  const Uint8 *keys = SDL_GetKeyboardState(&num_keys);
  const Uint32 now = SDL_GetTicks();

  const bool up = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W];
  const bool down = keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S];
  const bool left = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A];
  const bool right = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D];

  const bool fireUp = up_.tick(up, now / 1000.0);
  const bool fireDown = down_.tick(down, now / 1000.0);
  const bool fireLeft = left_.tick(left, now / 1000.0);
  const bool fireRight = right_.tick(right, now / 1000.0);

  if (fireUp)
    in.dir = Dir::Up;
  else if (fireDown)
    in.dir = Dir::Down;
  else if (fireLeft)
    in.dir = Dir::Left;
  else if (fireRight)
    in.dir = Dir::Right;

  const bool accept = keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_SPACE];
  const bool deny = keys[SDL_SCANCODE_ESCAPE];
  const bool clipboard = keys[SDL_SCANCODE_C];
  const bool menu = keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT];

  if (accept_.tick(accept))
    in.button |= Input::ACCEPT;
  if (deny_.tick(deny))
    in.button |= Input::DENY;
  if (clipboard_.tick(clipboard))
    in.button |= Input::ADD_TO_CLIPBOARD;
  if (menu_.tick(menu))
    in.button |= Input::OPEN_MENU_BAR;

  (void)num_keys;
  return in;
}
