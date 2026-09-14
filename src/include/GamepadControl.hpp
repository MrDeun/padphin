#pragma once

#include "Gate.hpp"
#include "IControl.hpp"
#include "SDL_gamecontroller.h"

#include <SDL2/SDL.h>

class GamepadControl final : public IControl {
private:
  SDL_GameController *controller_ = nullptr;

  RepeatGate up_, down_, left_, right_;
  EdgeGate accept_, deny_, clipboard_, menu_;

  static constexpr float kStickDeadzone = 0.5f;

  void close();
  void try_open_first();

public:
  GamepadControl();
  ~GamepadControl() override;

  Input poll() override;
  std::string get_device_name() const override;
  bool has_device() const override;
  DeviceType get_device_type() const override {
    if (!controller_) {
      return DeviceType::None;
    }
    auto type = SDL_GameControllerGetType(controller_);
    switch (type) {
    case SDL_CONTROLLER_TYPE_XBOX360:
    case SDL_CONTROLLER_TYPE_XBOXONE:
    case SDL_CONTROLLER_TYPE_AMAZON_LUNA:
    case SDL_CONTROLLER_TYPE_GOOGLE_STADIA:
    case SDL_CONTROLLER_TYPE_NVIDIA_SHIELD:
      return DeviceType::Xbox;
    case SDL_CONTROLLER_TYPE_PS3:
    case SDL_CONTROLLER_TYPE_PS4:
    case SDL_CONTROLLER_TYPE_PS5:
      return DeviceType::Playstation;
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
      return DeviceType::Switch;
    case SDL_CONTROLLER_TYPE_UNKNOWN:
    case SDL_CONTROLLER_TYPE_VIRTUAL:
    case SDL_CONTROLLER_TYPE_MAX:
    default:
      return DeviceType::None;
    }
    return DeviceType::None;
  }

  void on_controller_connected(SDL_JoystickID which);
  void on_controller_disconnected(SDL_JoystickID which);
};
