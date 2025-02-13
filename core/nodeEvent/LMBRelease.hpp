#pragma once

#include "core/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBRelease : public INEvent
{
    explicit LMBRelease()
    {}
};
} // namespace msgui::nodeevent
