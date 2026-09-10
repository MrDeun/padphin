#pragma once
#include "IControl.hpp"
#include <memory>
#include <vector>
class CompositeControl final : public IControl {
public:
    explicit CompositeControl(std::vector<std::unique_ptr<IControl>> sources)
        : sources_(std::move(sources)) {}

    Input poll() override;
    std::string get_device_name() const override;
    bool has_device() const override;
    DeviceType get_device_type() const override;

private:
    std::vector<std::unique_ptr<IControl>> sources_;
    DeviceType last_device_ = DeviceType::None;
};
