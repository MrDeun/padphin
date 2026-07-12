#pragma once

#include <QObject>
#include <QString>
#include <SDL3/SDL.h>

// Wraps a single connected SDL_Gamepad instance as a QObject.
// Owned/managed by SdlGamepadManager - you normally don't create this yourself.
// Registered for QML with qmlRegisterUncreatableType in main.cpp: QML can
// read/bind to instances handed to it by the manager, but can't do
// "SdlGamepad {}" itself.
class SdlGamepad : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(int instanceId READ instanceId CONSTANT)
  Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

  Q_PROPERTY(float leftStickX READ leftStickX NOTIFY leftStickChanged)
  Q_PROPERTY(float leftStickY READ leftStickY NOTIFY leftStickChanged)
  Q_PROPERTY(float rightStickX READ rightStickX NOTIFY rightStickChanged)
  Q_PROPERTY(float rightStickY READ rightStickY NOTIFY rightStickChanged)
  Q_PROPERTY(float leftTrigger READ leftTrigger NOTIFY triggersChanged)
  Q_PROPERTY(float rightTrigger READ rightTrigger NOTIFY triggersChanged)

public:
  // Mirrors SDL_GamepadButton as a Q_ENUM so QML can use e.g.
  // SdlGamepad.ButtonSouth. Extend with the remaining SDL_GAMEPAD_BUTTON_*
  // values (paddles, misc, touchpad) if you need them.
  enum class Button {
    ButtonSouth = SDL_GAMEPAD_BUTTON_SOUTH,
    ButtonEast = SDL_GAMEPAD_BUTTON_EAST,
    ButtonWest = SDL_GAMEPAD_BUTTON_WEST,
    ButtonNorth = SDL_GAMEPAD_BUTTON_NORTH,
    ButtonBack = SDL_GAMEPAD_BUTTON_BACK,
    ButtonGuide = SDL_GAMEPAD_BUTTON_GUIDE,
    ButtonStart = SDL_GAMEPAD_BUTTON_START,
    ButtonLeftStick = SDL_GAMEPAD_BUTTON_LEFT_STICK,
    ButtonRightStick = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
    ButtonLeftShoulder = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
    ButtonRightShoulder = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    ButtonDpadUp = SDL_GAMEPAD_BUTTON_DPAD_UP,
    ButtonDpadDown = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
    ButtonDpadLeft = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
    ButtonDpadRight = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
  };
  Q_ENUM(Button)

  enum class Axis {
    AxisLeftX = SDL_GAMEPAD_AXIS_LEFTX,
    AxisLeftY = SDL_GAMEPAD_AXIS_LEFTY,
    AxisRightX = SDL_GAMEPAD_AXIS_RIGHTX,
    AxisRightY = SDL_GAMEPAD_AXIS_RIGHTY,
    AxisLeftTrigger = SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
    AxisRightTrigger = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
  };
  Q_ENUM(Axis)

  enum class GamepadType {
    UNKNOWN = SDL_GAMEPAD_TYPE_UNKNOWN,
    STANDARD = SDL_GAMEPAD_TYPE_STANDARD,
    XBOX_360 = SDL_GAMEPAD_TYPE_XBOX360,
    XBOX_ONE = SDL_GAMEPAD_TYPE_XBOXONE,
    PS3 = SDL_GAMEPAD_TYPE_PS3,
    PS4 = SDL_GAMEPAD_TYPE_PS4,
    PS5 = SDL_GAMEPAD_TYPE_PS5,
    SWITCH_PRO = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
    JOYCON_LEFT = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT,
    JOYCON_RIGHT = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT,
    JOYCON_PAIR = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR,
    GAMECUBE = SDL_GAMEPAD_TYPE_GAMECUBE,
    // STEAM_CONTROLLER = SDL_GAMEPAD_TYPE_STEAM,
    COUNT = SDL_GAMEPAD_TYPE_COUNT
  };
  Q_ENUM(GamepadType);

  explicit SdlGamepad(SDL_Gamepad *gamepad, SDL_JoystickID instanceId,
                      QObject *parent = nullptr);
  ~SdlGamepad() override;

  QString name() const { return m_name; }
  int instanceId() const { return m_instanceId; }
  bool isConnected() const { return m_connected; }

  float leftStickX() const { return m_leftStickX; }
  float leftStickY() const { return m_leftStickY; }
  float rightStickX() const { return m_rightStickX; }
  float rightStickY() const { return m_rightStickY; }
  float leftTrigger() const { return m_leftTrigger; }
  float rightTrigger() const { return m_rightTrigger; }

  // Poll current state directly (in addition to the signals/properties above)
  Q_INVOKABLE bool isButtonPressed(SdlGamepad::Button button) const;
  Q_INVOKABLE float axisValue(SdlGamepad::Axis axis) const;

signals:
  void buttonPressed(SdlGamepad::Button button);
  void buttonReleased(SdlGamepad::Button button);
  void axisMoved(SdlGamepad::Axis axis,
                 float value); // normalized -1..1 (0..1 for triggers)
  void connectedChanged(bool connected);
  void leftStickChanged();
  void rightStickChanged();
  void triggersChanged();

private:
  friend class SdlGamepadManager;

  // Called by the manager when it dispatches an SDL event for this device.
  void handleButtonEvent(SDL_GamepadButton button, bool pressed);
  void handleAxisEvent(SDL_GamepadAxis axis, Sint16 rawValue);
  void setConnected(bool connected);

  static float normalizeAxis(SDL_GamepadAxis axis, Sint16 raw);
  static Axis toAxis(SDL_GamepadAxis axis) { return static_cast<Axis>(axis); }
  static Button toButton(SDL_GamepadButton button) {
    return static_cast<Button>(button);
  }

  SDL_Gamepad *m_gamepad = nullptr;
  SDL_JoystickID m_instanceId = 0;
  GamepadType m_type = GamepadType::UNKNOWN;

  QString m_name;
  bool m_connected = true;

  float m_leftStickX = 0.0f;
  float m_leftStickY = 0.0f;
  float m_rightStickX = 0.0f;
  float m_rightStickY = 0.0f;
  float m_leftTrigger = 0.0f;
  float m_rightTrigger = 0.0f;
};