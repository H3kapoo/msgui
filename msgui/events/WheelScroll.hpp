#pragma once

#include <cstdint>

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct WheelScroll : public INEvent
{
    explicit WheelScroll(int32_t valueIn)
        : value{valueIn}
    {}

    int32_t value{0};
};
} // namespace msgui::events
