#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct MouseEnter : public INEvent
{
    explicit MouseEnter()
    {}
};
} // namespace msgui::events
