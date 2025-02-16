#pragma once

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBClick : public INEvent
{
    explicit LMBClick()
    {}
};
} // namespace msgui::nodeevent
