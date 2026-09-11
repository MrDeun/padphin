#pragma once
#include <bitset>
#include <cstdint>
#include <string>

enum class Dir : uint8_t { None, Up, Down, Left, Right };
enum class DeviceType : uint8_t { Keyboard, Xbox, Switch, Playstation, None };

struct Input {
    Dir dir = Dir::None;
    uint8_t button = 0;
    DeviceType device = DeviceType::None;
    static constexpr uint8_t ACCEPT = 1 << 0;
    static constexpr uint8_t DENY = 1 << 1;
    static constexpr uint8_t ADD_TO_CLIPBOARD = 1 << 2;
    static constexpr uint8_t OPEN_MENU_BAR = 1 << 3;
};
class IControl {
    public:
    virtual Input poll() = 0;
    virtual std::string get_device_name() const { return ""; }
    virtual bool has_device() const { return false; }
    virtual DeviceType get_device_type() const { return DeviceType::None; }
    virtual ~IControl() = default;
};
