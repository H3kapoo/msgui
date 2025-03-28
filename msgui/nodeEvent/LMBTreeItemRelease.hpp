#pragma once

#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBTreeItemRelease : public INEvent
{
    explicit LMBTreeItemRelease(TreeItemWPtr treeItem)
        : item{treeItem}
    {}

    TreeItemWPtr item{};
};
} // namespace msgui::nodeevent
