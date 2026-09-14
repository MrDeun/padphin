#pragma once

#include "Gate.hpp"
#include "IControl.hpp"

#include <SDL2/SDL.h>

class KeyboardControl final : public IControl {
public:
    explicit KeyboardControl(SDL_Window* window);

    Input poll() override;
    DeviceType get_device_type() const override { return DeviceType::Keyboard; }

private:
    SDL_Window* window_;

    RepeatGate up_, down_, left_, right_;
    EdgeGate accept_, deny_, clipboard_, menu_;
};
