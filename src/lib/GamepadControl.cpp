#include "GamepadControl.hpp"

GamepadControl::GamepadControl() { try_open_first(); }

GamepadControl::~GamepadControl() { close(); }

void GamepadControl::close() {
  if (controller_) {
    SDL_GameControllerClose(controller_);
    controller_ = nullptr;
  }
}

void GamepadControl::try_open_first() {
  close();
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      controller_ = SDL_GameControllerOpen(i);
      if (controller_)
        return;
    }
  }
}

std::string GamepadControl::get_device_name() const {
  if (!controller_ || !SDL_GameControllerGetAttached(controller_))
    return "";
  const char *name = SDL_GameControllerName(controller_);
  return name ? name : "Unknown Controller";
}

bool GamepadControl::has_device() const {
  return controller_ != nullptr && SDL_GameControllerGetAttached(controller_);
}

void GamepadControl::on_controller_connected(SDL_JoystickID which) {
  if (!controller_) {
    SDL_GameController *gc = SDL_GameControllerFromInstanceID(which);
    if (gc)
      controller_ = gc;
    else
      try_open_first();
  }
}

void GamepadControl::on_controller_disconnected(SDL_JoystickID which) {
  if (controller_ && SDL_JoystickInstanceID(
                         SDL_GameControllerGetJoystick(controller_)) == which) {
    close();
    try_open_first();
  }
}

Input GamepadControl::poll() {
  Input in;

  if (!controller_ || !SDL_GameControllerGetAttached(controller_)) {
    close();
    try_open_first();
    up_.reset();
    down_.reset();
    left_.reset();
    right_.reset();
    return in;
  }

  const Uint32 now = SDL_GetTicks();
  const double now_s = now / 1000.0;

  bool up =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_DPAD_UP);
  bool down =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  bool left =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  bool right = SDL_GameControllerGetButton(controller_,
                                           SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

  if (!up && !down && !left && !right) {
    Sint16 ax =
        SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_LEFTX);
    Sint16 ay =
        SDL_GameControllerGetAxis(controller_, SDL_CONTROLLER_AXIS_LEFTY);
    float nax = ax / 32767.0f;
    float nay = ay / 32767.0f;
    if (nay < -kStickDeadzone)
      up = true;
    else if (nay > kStickDeadzone)
      down = true;
    if (nax < -kStickDeadzone)
      left = true;
    else if (nax > kStickDeadzone)
      right = true;
  }

  const bool fireUp = up_.tick(up, now_s);
  const bool fireDown = down_.tick(down, now_s);
  const bool fireLeft = left_.tick(left, now_s);
  const bool fireRight = right_.tick(right, now_s);

  if (fireUp)
    in.dir = Dir::Up;
  else if (fireDown)
    in.dir = Dir::Down;
  else if (fireLeft)
    in.dir = Dir::Left;
  else if (fireRight)
    in.dir = Dir::Right;

  const bool accept =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_A);
  const bool deny =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_B);
  const bool clipboard =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_X);
  const bool menu =
      SDL_GameControllerGetButton(controller_, SDL_CONTROLLER_BUTTON_Y);

  if (accept_.tick(accept))
    in.button |= Input::ACCEPT;
  if (deny_.tick(deny))
    in.button |= Input::DENY;
  if (clipboard_.tick(clipboard))
    in.button |= Input::ADD_TO_CLIPBOARD;
  if (menu_.tick(menu))
    in.button |= Input::OPEN_MENU_BAR;

  return in;
}
