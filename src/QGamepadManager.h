#pragma once

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QList>
#include <SDL3/SDL.h>

#include "QGamepad.h"
// Owns SDL's gamepad subsystem, pumps SDL_PollEvent on a QTimer so it
// integrates with Qt's event loop (no separate thread needed), and creates/
// destroys SdlGamepad QObjects as controllers are hot-plugged.
//
// Exposed to QML as a context property (see main.cpp) - not a QML singleton,
// because start()/stop() need to be driven explicitly from C++. Registered
// with qmlRegisterUncreatableType purely so QML can see it as a known type.
class SdlGamepadManager : public QObject
{
    Q_OBJECT

public:
    explicit SdlGamepadManager(QObject *parent = nullptr);
    ~SdlGamepadManager() override;

    // Starts SDL init + polling. pollIntervalMs defaults to ~60Hz.
    bool start(int pollIntervalMs = 16);
    void stop();

    QList<SdlGamepad *> gamepads() const { return m_gamepadList; }
    // Convenience for simple single-controller UIs: first connected gamepad, or nullptr.
    SdlGamepad *primaryGamepad() const { return m_gamepadList.isEmpty() ? nullptr : m_gamepadList.first(); }

    SdlGamepad *gamepad(SDL_JoystickID instanceId) const { return m_gamepads.value(instanceId, nullptr); }

signals:
    void gamepadConnected(SdlGamepad *gamepad);
    void gamepadDisconnected(SdlGamepad *gamepad); // emitted just before deletion
    void gamepadsChanged();

private slots:
    void poll();

private:
    void handleDeviceAdded(SDL_JoystickID instanceId);
    void handleDeviceRemoved(SDL_JoystickID instanceId);

    QTimer m_pollTimer;
    QMap<SDL_JoystickID, SdlGamepad *> m_gamepads;
    QList<SdlGamepad *> m_gamepadList; // stable ordering, backs the QQmlListProperty
    bool m_sdlInitialized = false;
};