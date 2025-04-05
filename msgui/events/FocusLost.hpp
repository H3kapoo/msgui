#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct FocusLost : public INEvent
{
    explicit FocusLost()
    {}
};
} // namespace msgui::events
