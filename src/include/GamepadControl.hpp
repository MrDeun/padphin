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
  DeviceType get_device_type() const override ;

  void on_controller_connected(SDL_JoystickID which);
  void on_controller_disconnected(SDL_JoystickID which);
};
