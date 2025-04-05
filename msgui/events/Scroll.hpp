#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct Scroll : public INEvent
{
    explicit Scroll(float valueIn)
        : value{valueIn}
    {}

    float value{0};
};
} // namespace msgui::events
