#pragma once

#include "core/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBClick : public INEvent
{
    explicit LMBClick()
    {}
};
} // namespace msgui::nodeevent
