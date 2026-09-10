#pragma once
class RepeatGate {
public:
    bool tick(bool heldNow, double now, double firstDelay = 0.35, double repeatInterval = 0.10);
    void reset() { held_ = false; }

private:
    bool held_ = false;
    double changedAt_ = 0.0;
    double lastFire_ = 0.0;
};

// Generic edge detector for buttons: fires exactly once per press.
class EdgeGate {
public:
    bool tick(bool heldNow);

private:
    bool prev_ = false;
};
