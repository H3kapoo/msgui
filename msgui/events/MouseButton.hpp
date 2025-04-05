#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct MouseButton : public INEvent
{
    explicit MouseButton()
    {}
};
} // namespace msgui::events
