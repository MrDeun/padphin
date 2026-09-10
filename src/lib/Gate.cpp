#include "Gate.hpp"
bool RepeatGate::tick(bool heldNow, double now, double firstDelay, double repeatInterval) {
    if (!heldNow) {
        held_ = false;
        return false;
    }
    if (!held_) {
        // Fresh press: fire immediately.
        held_ = true;
        changedAt_ = now;
        lastFire_ = now;
        return true;
    }
    const double heldFor = now - changedAt_;
    if (heldFor < firstDelay) return false;
    if (now - lastFire_ >= repeatInterval) {
        lastFire_ = now;
        return true;
    }
    return false;
}

bool EdgeGate::tick(bool heldNow) {
    const bool fire = heldNow && !prev_;
    prev_ = heldNow;
    return fire;
}
