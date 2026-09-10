
#include "CompositeControl.hpp"
Input CompositeControl::poll() {
    Input result;
    for (auto& src : sources_) {
        Input in = src->poll(); // always poll every source, even after a match,
                                 // so held-key state / edge gates stay in sync.
        if (result.dir == Dir::None) result.dir = in.dir;
        result.button |= in.button;
        if (in.dir != Dir::None || in.button != 0) {
            DeviceType type = src->get_device_type();
            if (type != DeviceType::None) {
                last_device_ = type;
                result.device = type;
            }
        }
    }
    if (result.device == DeviceType::None) {
        result.device = last_device_;
    }
    return result;
}

std::string CompositeControl::get_device_name() const {
    for (const auto &src : sources_) {
        if (src->has_device())
            return src->get_device_name();
    }
    return "";
}

bool CompositeControl::has_device() const {
    for (const auto &src : sources_) {
        if (src->has_device())
            return true;
    }
    return false;
}

DeviceType CompositeControl::get_device_type() const {
    return last_device_;
}
