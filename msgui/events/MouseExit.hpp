#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct MouseExit : public INEvent
{
    explicit MouseExit()
    {}
};
} // namespace msgui::events
