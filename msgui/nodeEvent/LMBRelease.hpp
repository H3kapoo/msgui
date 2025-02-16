#pragma once

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBRelease : public INEvent
{
    explicit LMBRelease()
    {}
};
} // namespace msgui::nodeevent
