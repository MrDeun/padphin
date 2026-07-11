#include "QGamepad.h"

SdlGamepad::SdlGamepad(SDL_Gamepad *gamepad, SDL_JoystickID instanceId, QObject *parent)
    : QObject(parent)
    , m_gamepad(gamepad)
    , m_instanceId(instanceId)
{
    const char *n = SDL_GetGamepadName(gamepad);
    m_name = n ? QString::fromUtf8(n) : QStringLiteral("Unknown Gamepad");
}

SdlGamepad::~SdlGamepad()
{
    if (m_gamepad) {
        SDL_CloseGamepad(m_gamepad);
        m_gamepad = nullptr;
    }
}

bool SdlGamepad::isButtonPressed(SdlGamepad::Button button) const
{
    if (!m_gamepad || !m_connected)
        return false;
    return SDL_GetGamepadButton(m_gamepad, static_cast<SDL_GamepadButton>(button));
}

float SdlGamepad::axisValue(SdlGamepad::Axis axis) const
{
    if (!m_gamepad || !m_connected)
        return 0.0f;
    auto sdlAxis = static_cast<SDL_GamepadAxis>(axis);
    return normalizeAxis(sdlAxis, SDL_GetGamepadAxis(m_gamepad, sdlAxis));
}

void SdlGamepad::handleButtonEvent(SDL_GamepadButton button, bool pressed)
{
    if (pressed)
        emit buttonPressed(toButton(button));
    else
        emit buttonReleased(toButton(button));
}

void SdlGamepad::handleAxisEvent(SDL_GamepadAxis axis, Sint16 rawValue)
{
    const float value = normalizeAxis(axis, rawValue);
    emit axisMoved(toAxis(axis), value);

    // Update cached values + fire the grouped property-change signals QML binds to.
    switch (axis) {
    case SDL_GAMEPAD_AXIS_LEFTX:
        m_leftStickX = value;
        emit leftStickChanged();
        break;
    case SDL_GAMEPAD_AXIS_LEFTY:
        m_leftStickY = value;
        emit leftStickChanged();
        break;
    case SDL_GAMEPAD_AXIS_RIGHTX:
        m_rightStickX = value;
        emit rightStickChanged();
        break;
    case SDL_GAMEPAD_AXIS_RIGHTY:
        m_rightStickY = value;
        emit rightStickChanged();
        break;
    case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
        m_leftTrigger = value;
        emit triggersChanged();
        break;
    case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
        m_rightTrigger = value;
        emit triggersChanged();
        break;
    default:
        break;
    }
}

void SdlGamepad::setConnected(bool connected)
{
    if (m_connected == connected)
        return;
    m_connected = connected;
    emit connectedChanged(connected);
}

float SdlGamepad::normalizeAxis(SDL_GamepadAxis axis, Sint16 raw)
{
    // Triggers report 0..32767, sticks report -32768..32767
    if (axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER || axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
        return static_cast<float>(raw) / 32767.0f;

    return raw < 0 ? static_cast<float>(raw) / 32768.0f
                    : static_cast<float>(raw) / 32767.0f;
}