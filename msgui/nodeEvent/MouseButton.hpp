#pragma once

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct MouseButton : public INEvent
{
    explicit MouseButton()
    {}
};
} // namespace msgui::nodeevent
