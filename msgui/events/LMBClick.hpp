#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct LMBClick : public INEvent
{
    explicit LMBClick()
    {}
};
} // namespace msgui::events
