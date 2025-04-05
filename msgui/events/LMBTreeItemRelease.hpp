#pragma once

#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct LMBTreeItemRelease : public INEvent
{
    explicit LMBTreeItemRelease(TreeItemWPtr treeItem)
        : item{treeItem}
    {}

    TreeItemWPtr item{};
};
} // namespace msgui::events
