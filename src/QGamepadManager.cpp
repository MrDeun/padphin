#include "QGamepadManager.h"

#include <QDebug>

SdlGamepadManager::SdlGamepadManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_pollTimer, &QTimer::timeout, this, &SdlGamepadManager::poll);
}

SdlGamepadManager::~SdlGamepadManager()
{
    stop();
}

bool SdlGamepadManager::start(int pollIntervalMs)
{
    if (m_sdlInitialized)
        return true;

    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
        qWarning() << "SDL_InitSubSystem(SDL_INIT_GAMEPAD) failed:" << SDL_GetError();
        return false;
    }
    m_sdlInitialized = true;

    // Pick up any gamepads that were already connected before start() was called.
    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);
    if (ids) {
        for (int i = 0; i < count; ++i)
            handleDeviceAdded(ids[i]);
        SDL_free(ids);
    }

    m_pollTimer.start(pollIntervalMs);
    return true;
}

void SdlGamepadManager::stop()
{
    m_pollTimer.stop();

    qDeleteAll(m_gamepads);
    m_gamepads.clear();
    m_gamepadList.clear();

    if (m_sdlInitialized) {
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
        m_sdlInitialized = false;
    }
}

void SdlGamepadManager::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_GAMEPAD_ADDED:
            handleDeviceAdded(event.gdevice.which);
            break;

        case SDL_EVENT_GAMEPAD_REMOVED:
            handleDeviceRemoved(event.gdevice.which);
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            if (SdlGamepad *gp = m_gamepads.value(event.gbutton.which)) {
                gp->handleButtonEvent(static_cast<SDL_GamepadButton>(event.gbutton.button),
                                       event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN);
            }
            break;

        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            if (SdlGamepad *gp = m_gamepads.value(event.gaxis.which)) {
                gp->handleAxisEvent(static_cast<SDL_GamepadAxis>(event.gaxis.axis),
                                     event.gaxis.value);
            }
            break;

        default:
            break;
        }
    }
}

void SdlGamepadManager::handleDeviceAdded(SDL_JoystickID instanceId)
{
    if (m_gamepads.contains(instanceId))
        return;

    SDL_Gamepad *raw = SDL_OpenGamepad(instanceId);
    if (!raw) {
        qWarning() << "SDL_OpenGamepad failed:" << SDL_GetError();
        return;
    }

    auto *gp = new SdlGamepad(raw, instanceId, this);
    m_gamepads.insert(instanceId, gp);
    m_gamepadList.append(gp);
    emit gamepadConnected(gp);
    emit gamepadsChanged();
}

void SdlGamepadManager::handleDeviceRemoved(SDL_JoystickID instanceId)
{
    SdlGamepad *gp = m_gamepads.take(instanceId);
    if (!gp)
        return;

    m_gamepadList.removeOne(gp);
    gp->setConnected(false);
    emit gamepadDisconnected(gp);
    emit gamepadsChanged();
    gp->deleteLater();
}