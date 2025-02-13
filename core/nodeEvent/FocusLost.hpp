#pragma once

#include "core/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct FocusLost : public INEvent
{
    explicit FocusLost()
    {}
};
} // namespace msgui::nodeevent
