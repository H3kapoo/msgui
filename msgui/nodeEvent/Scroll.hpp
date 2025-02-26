#pragma once

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct Scroll : public INEvent
{
    explicit Scroll(float valueIn)
        : value{valueIn}
    {}

    float value{0};
};
} // namespace msgui::nodeevent
