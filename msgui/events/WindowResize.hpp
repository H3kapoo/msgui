#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct WindowResize : public INEvent
{
    explicit WindowResize()
    {}
};
} // namespace msgui::events
