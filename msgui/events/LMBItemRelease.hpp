#pragma once

#include "msgui/events/INodeEvent.hpp"
#include "msgui/node/utils/ListItem.hpp"

namespace msgui::events
{
struct LMBItemRelease : public INEvent
{
    explicit LMBItemRelease(node::utils::ListItem* itemIn)
        : item{itemIn}
    {}

    node::utils::ListItem* item;
};
} // namespace msgui::events
